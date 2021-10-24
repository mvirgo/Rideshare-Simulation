/**
 * @file vehicle_manager.cpp
 * @brief Implementation of generating vehicles and driving them around the map.
 *
 * @copyright Copyright (c) 2021, Michael Virgo, released under the MIT License.
 *
 */

#include "vehicle_manager.h"

#include <memory>

#include "ride_matcher.h"
#include "mapping/coordinate.h"
#include "mapping/route_model.h"
#include "map_object/passenger.h"
#include "map_object/vehicle.h"
#include "routing/route_planner.h"

namespace rideshare {

VehicleManager::VehicleManager(RouteModel *model,
                               std::shared_ptr<RoutePlanner> route_planner,
                               int max_objects) : ObjectHolder(model, route_planner, max_objects) {
    // Set distance per cycle based on model's latitudes
    distance_per_cycle_ = std::abs(model_->MaxLat() - model->MinLat()) / 1000.0;
    // Generate max number of vehicles at the start
    for (int i = 0; i < MAX_OBJECTS_; ++i) {
        GenerateNew();
    }
}

void VehicleManager::GenerateNew() {
    // Get random start position
    auto start = model_->GetRandomMapPosition();
    // Set a random destination until they have a passenger to go pick up
    auto destination = model_->GetRandomMapPosition();
    // Find the nearest road node to start and destination positions
    auto nearest_start = model_->FindClosestNode(start);
    auto nearest_dest = model_->FindClosestNode(destination);
    // Set road position, destination and id of vehicle
    std::shared_ptr<Vehicle> vehicle = std::make_shared<Vehicle>(distance_per_cycle_);
    vehicle->SetPosition((Coordinate){.x = nearest_start.x, .y = nearest_start.y});
    vehicle->SetDestination((Coordinate){.x = nearest_dest.x, .y = nearest_dest.y});
    vehicle->SetId(idCnt_++);
    vehicles_.emplace(vehicle->Id(), vehicle);
    // Output id and location of vehicle looking to give rides
    std::lock_guard<std::mutex> lck(mtx_);
    std::cout << "Vehicle #" << idCnt_ - 1 << " now driving from: " << nearest_start.y << ", " << nearest_start.x << "." << std::endl;
}

void VehicleManager::ResetVehicleDestination(std::shared_ptr<Vehicle> vehicle, bool random) {
    Coordinate destination;
    // Depending on `random`, either get a new random position or set current destination onto nearest node
    if (random) {
        destination = model_->GetRandomMapPosition();
    } else {
        destination = vehicle->GetDestination();
    }
    auto nearest_dest = model_->FindClosestNode(destination);
    vehicle->SetDestination((Coordinate){.x = nearest_dest.x, .y = nearest_dest.y});
}

void VehicleManager::Simulate() {
    // Launch Drive function in a thread
    threads.emplace_back(std::thread(&VehicleManager::Drive, this));
}

void VehicleManager::Drive() {
    while (true) {
        // Sleep at every iteration to reduce CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        // Pick up any available passengers first
        PickUpPassengers();
        // Assign any new matches
        NewPassengerAssignments();

        // Drive the vehicles
        for (auto & [id, vehicle] : vehicles_) {
            // Get a route if none yet given
            if (vehicle->Path().empty()) {
                route_planner_->AStarSearch(vehicle);
                if (vehicle->Path().empty()) {
                    if (vehicle->State() == VehicleState::no_passenger_requested || vehicle->State() == VehicleState::no_passenger_queued) {
                        SimpleVehicleFailure(vehicle);
                        continue;
                    }
                }
            }

            // Request a passenger if don't have one yet
            if (vehicle->State() == VehicleState::no_passenger_requested) {
                RequestPassenger(vehicle);
            }

            // Drive to destination or wait, depending on state
            if (vehicle->State() == VehicleState::waiting) {
                continue;
            } else {
                // Drive to current destination
                vehicle->IncrementalMove();
            }

            // Check if at destination
            if (vehicle->GetPosition() == vehicle->GetDestination()) {
                if (vehicle->State() == VehicleState::no_passenger_queued) {
                    // Find a new random destination
                    ResetVehicleDestination(vehicle, true);
                } else if (vehicle->State() == VehicleState::passenger_queued) {
                    // Notify of arrival
                    ArrivedAtPassenger(vehicle);
                } else if (vehicle->State() == VehicleState::driving_passenger) {
                    // Drop-off passenger
                    DropOffPassenger(vehicle);
                }
            }
        }

        // Remove any vehicles that had issues on the map
        if (to_remove_.size() > 0) {
            for (int id : to_remove_) {
                // Notify ride matcher (doesn't matter for no request state or driving, but does for others)
                ride_matcher_->Message({ .message_code=RideMatcher::vehicle_is_ineligible, .id=id });
                // Erase the vehicle
                vehicles_.erase(id);
            }
            // Clear the to_remove_ vector for next time
            to_remove_.clear();
        }

        // Make sure to keep max vehicles on the road
        if (vehicles_.size() < MAX_OBJECTS_) {
            GenerateNew();
        }
    }
}

void VehicleManager::SimpleVehicleFailure(std::shared_ptr<Vehicle> vehicle) {
    // Note: This should only be called when vehicle has not yet picked up a passenger
    // Check if enough failures to delete
    bool remove = vehicle->MovementFailure();
    if (remove) {
        // Plan to erase the vehicle
        to_remove_.emplace_back(vehicle->Id());
        // Note to console
        std::lock_guard<std::mutex> lck(mtx_);
        std::cout << "Vehicle #" << vehicle->Id() <<" is stuck, leaving map." << std::endl;
    } else {
        // Try a new route
        ResetVehicleDestination(vehicle, true);
    }
}

void VehicleManager::RequestPassenger(std::shared_ptr<Vehicle> vehicle) {
    // Update state first (make sure no async issues)
    vehicle->SetState(VehicleState::no_passenger_queued);
    // Request the passenger from the ride matcher
    if (ride_matcher_ != nullptr) {
        ride_matcher_->Message({ .message_code=RideMatcher::vehicle_requests_passenger, .id=vehicle->Id() });
    }
}

void VehicleManager::AssignPassenger(int id, Coordinate position) {
    std::lock_guard<std::mutex> lck(new_assignment_locations_mutex);
    // Add the newly assigned passenger pickup position for later use
    new_assignment_locations.emplace(id, position);
}

void VehicleManager::NewPassengerAssignments() {
    // Lock and copy over the new assignments so can release the mutex faster
    std::unique_lock<std::mutex> lck(new_assignment_locations_mutex);
    std::unordered_map<int, Coordinate> copied_assignments(new_assignment_locations);
    // Clear out the new assignment locations and unlock
    new_assignment_locations.clear();
    lck.unlock();

    // Loop through an assign passenger pick up locations to related vehicles
    for (auto [id, position] : copied_assignments) {
        auto vehicle = vehicles_.at(id);
        // Store current position
        Coordinate curr_pos = vehicle->GetPosition();
        // Set position for use with route to passenger as the next node on the path
        // Avoids potential issue if current position is closest to an unreachable node
        if (vehicle->Path().empty()) {
            // Empty path likely a result of failure, so don't progress with assignment
            AssignmentFailure(vehicle);
            return;
        }
        Model::Node next_node = vehicle->Path().at(vehicle->PathIndex());
        vehicle->SetPosition({ .x = next_node.x, .y = next_node.y });
        // Set new vehicle destination and update its state
        vehicle->SetDestination(position);
        ResetVehicleDestination(vehicle, false); // Aligns to route node
        // Get the path to the passenger
        route_planner_->AStarSearch(vehicle);
        // Set position back to original to keep smooth route
        vehicle->SetPosition(curr_pos);
        // Make sure path is not empty (unreachable), then update the state
        if (vehicle->Path().empty()) {
            AssignmentFailure(vehicle);
        } else {
            // Update state when done processing
            vehicle->SetState(VehicleState::passenger_queued);
        }
    }
}

void VehicleManager::AssignmentFailure(std::shared_ptr<Vehicle> vehicle) {
    // Notify ride matcher of failure
    ride_matcher_->Message({ .message_code=RideMatcher::vehicle_cannot_reach_passenger, .id=vehicle->Id() });
    // Set state to nothing requested so it will make a new request
    vehicle->SetState(VehicleState::no_passenger_requested);
    // Add to vehicle failures
    // Note that ride matcher notified in `Drive` if deletion occurs
    SimpleVehicleFailure(vehicle);
}

void VehicleManager::ArrivedAtPassenger(std::shared_ptr<Vehicle> vehicle) {
    // Transition to waiting
    vehicle->SetState(VehicleState::waiting);
    // Notify ride matcher
    ride_matcher_->Message({ .message_code=RideMatcher::vehicle_has_arrived, .id=vehicle->Id() });
}

void VehicleManager::PassengerIntoVehicle(int id, std::shared_ptr<Passenger> passenger) {
    std::lock_guard<std::mutex> pickups_lock(passenger_pickups_mutex);
    // Add to passenger pickups map
    passenger_pickups_.emplace(id, passenger);
}

void VehicleManager::PickUpPassengers() {
    // Lock and copy over the passenger pickups so can release the mutex faster
    std::unique_lock<std::mutex> pickups_lock(passenger_pickups_mutex);
    std::unordered_map<int, std::shared_ptr<Passenger>> copied_pickups(passenger_pickups_);
    // Clear out the passenger pickups and unlock
    passenger_pickups_.clear();
    pickups_lock.unlock();

    // Loop through all ready passenger pickups
    for (auto [id, passenger] : copied_pickups) {
        auto vehicle = vehicles_.at(id);
        // Output notice to console
        std::unique_lock<std::mutex> lck(mtx_);
        std::cout << "Vehicle #" << vehicle->Id() << " picked up Passenger #" << passenger->Id() << "." << std::endl;
        lck.unlock();
        // Set passenger into vehicle
        vehicle->SetPassenger(passenger); // Vehicle handles setting new destination with passenger
        ResetVehicleDestination(vehicle, false); // Aligns to route node
        // Update state when done processing
        vehicle->SetState(VehicleState::driving_passenger);
    }
}

void VehicleManager::DropOffPassenger(std::shared_ptr<Vehicle> vehicle) {
    // Output notice to console
    std::unique_lock<std::mutex> lck(mtx_);
    std::cout << "Vehicle #" << vehicle->Id() << " dropped off Passenger #" << vehicle->GetPassenger()->Id() << "." << std::endl;
    lck.unlock();
    // Drop off the passenger
    vehicle->DropOffPassenger();
    // Find a new random destination
    ResetVehicleDestination(vehicle, true);
    // Transition back to no passenger requested state
    vehicle->SetState(VehicleState::no_passenger_requested);
}

}  // namespace rideshare
