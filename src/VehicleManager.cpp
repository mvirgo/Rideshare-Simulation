#include "VehicleManager.h"
#include <cmath>

#include "Coordinate.h"
#include "Passenger.h"
#include "RouteModel.h"
#include "RoutePlanner.h"
#include "RideMatcher.h"
#include "Vehicle.h"

namespace rideshare {

VehicleManager::VehicleManager(RouteModel *model, std::shared_ptr<RoutePlanner> route_planner) : ObjectHolder(model, route_planner) {
    // Generate max number of vehicles at the start
    for (int i = 0; i < MAX_OBJECTS; ++i) {
        GenerateNew();
    }
    // Set distance per cycle based on model's latitudes
    distance_per_cycle_ = std::abs(model_->MaxLat() - model->MinLat()) / 1000.0;
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
    std::shared_ptr<Vehicle> vehicle = std::make_shared<Vehicle>();
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

// Make for smooth, incremental driving between path nodes
void VehicleManager::IncrementalMove(std::shared_ptr<Vehicle> vehicle) {
    // Try to get the next_pos; an error here may happen due to a race condition
    //   where the path has been reset mid-stream
    Model::Node next_pos;
    try {
        next_pos = vehicle->Path().at(vehicle->PathIndex());
    } catch (...) {
        // Don't increment and instead return; it will be given a new path soon
        return;
    }
    // Check distance to next position vs. distance can go b/w timesteps
    auto pos = vehicle->GetPosition();
    auto distance = std::sqrt(std::pow(next_pos.x - pos.x, 2) + std::pow(next_pos.y - pos.y, 2));

    if (distance <= distance_per_cycle_) {
        // Don't need to calculate intermediate point, just set position as next_pos
        vehicle->SetPosition((Coordinate){.x = next_pos.x, .y = next_pos.y});
        vehicle->IncrementPathIndex();
    } else {
        // Calculate an intermediate position
        double angle = std::atan2(next_pos.y - pos.y, next_pos.x - pos.x); // angle from x-axis
        double new_pos_x = pos.x + (distance_per_cycle_ * std::cos(angle));
        double new_pos_y = pos.y + (distance_per_cycle_ * std::sin(angle));
        vehicle->SetPosition((Coordinate){.x = new_pos_x, .y = new_pos_y});
    }
}

void VehicleManager::Simulate() {
    // Launch Drive function in a thread
    threads.emplace_back(std::thread(&VehicleManager::Drive, this));
}

void VehicleManager::Drive() {
    while (true) {
        // Sleep at every iteration to reduce CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

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
                IncrementalMove(vehicle);
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
                ride_matcher_->VehicleIsIneligible(id);
                // Erase the vehicle
                vehicles_.erase(id);
            }
            // Clear the to_remove_ vector for next time
            to_remove_.clear();
        }

        // Make sure to keep max vehicles on the road
        if (vehicles_.size() < MAX_OBJECTS) {
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
        ride_matcher_->VehicleRequestsPassenger(vehicle->Id());
    }
}

void VehicleManager::AssignPassenger(int id, Coordinate position) {
    auto vehicle = vehicles_.at(id);
    // Set new vehicle destination and update its state
    vehicle->SetDestination(position);
    ResetVehicleDestination(vehicle, false); // Aligns to route node
    // Get the path to the passenger
    route_planner_->AStarSearch(vehicle);
    // Make sure path is not empty (unreachable), then update the state
    if (vehicle->Path().empty()) {
        // Notify ride matcher of failure
        ride_matcher_->VehicleCannotReachPassenger(id);
        // Set state to nothing requested so it will make a new request
        vehicle->SetState(VehicleState::no_passenger_requested);
        // Add to vehicle failures
        // Note that ride matcher notified in `Drive` if deletion occurs
        SimpleVehicleFailure(vehicle);
    } else {
        // Update state when done processing
        vehicle->SetState(VehicleState::passenger_queued);
    }
}

void VehicleManager::ArrivedAtPassenger(std::shared_ptr<Vehicle> vehicle) {
    // Transition to waiting
    vehicle->SetState(VehicleState::waiting);
    // Notify ride matcher
    ride_matcher_->VehicleHasArrived(vehicle->Id());
}

void VehicleManager::PassengerIntoVehicle(int id, std::shared_ptr<Passenger> passenger) {
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
