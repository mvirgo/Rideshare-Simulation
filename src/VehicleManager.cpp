#include "VehicleManager.h"
#include <cmath>
#include "RoutePlanner.h"
#include "RideMatcher.h"

VehicleManager::VehicleManager(RouteModel *model) : ConcurrentObject(model) {
    // TODO: Add simulation instead of creating vehicles at start?
    for (int i = 0; i < MAX_OBJECTS; ++i) {
        GenerateNew();
    }
    // Set distance per cycle based on model's latitudes
    distance_per_cycle_ = std::abs(model_->MaxLat() - model->MinLat()) / 10000.0;
}

void VehicleManager::GenerateNew() {
    // TODO: Add appropriate handling of Vehicle to avoid memory leaks if made a pointer
    // Get random start position
    auto start = model_->GetRandomMapPosition();
    // Set a random destination until they have a passenger to go pick up
    auto destination = model_->GetRandomMapPosition();
    // Find the nearest road node to start and destination positions
    auto nearest_start = model_->FindClosestNode(start[0], start[1]);
    auto nearest_dest = model_->FindClosestNode(destination[0], destination[1]);
    // Set road position, destination and id of vehicle
    Vehicle vehicle = Vehicle(); // TODO: May need change for memory changes later
    vehicle.SetPosition(nearest_start.x, nearest_start.y);
    vehicle.SetDestination(nearest_dest.x, nearest_dest.y);
    vehicle.SetId(idCnt_++);
    vehicles_.emplace_back(vehicle);
    // Output id and location of vehicle looking to give rides
    std::lock_guard<std::mutex> lck(mtx_);
    std::cout << "Vehicle ID#" << idCnt_ - 1 << " now driving from: " << nearest_start.y << ", " << nearest_start.x << "." << std::endl;
}

void VehicleManager::ResetVehicleDestination(Vehicle &vehicle, bool random) {
    std::vector<double> destination;
    // Depending on `random`, either get a new random position or set current destination onto nearest node
    if (random) {
        destination = model_->GetRandomMapPosition();
    } else {
        destination = vehicle.GetDestination();
    }
    auto nearest_dest = model_->FindClosestNode(destination[0], destination[1]);
    vehicle.SetDestination(nearest_dest.x, nearest_dest.y);
    // Reset the path and index so will properly route on a new path
    vehicle.ResetPathAndIndex();
}

// Make for smooth, incremental driving between path nodes
void VehicleManager::IncrementalMove(Vehicle &vehicle) {
    // Check distance to next position vs. distance can go b/w timesteps
    auto pos = vehicle.GetPosition();
    auto next_pos = vehicle.Path().at(vehicle.PathIndex());
    auto distance = std::sqrt(std::pow(next_pos.x - pos[0], 2) + std::pow(next_pos.y - pos[1], 2));

    if (distance <= distance_per_cycle_) {
        // Don't need to calculate intermediate point, just set position as next_pos
        vehicle.SetPosition(next_pos.x, next_pos.y);
        vehicle.IncrementPathIndex();
    } else {
        // Calculate an intermediate position
        double angle = std::atan2(next_pos.y - pos[1], next_pos.x - pos[0]); // angle from x-axis
        double new_pos_x = pos[0] + (distance_per_cycle_ * std::cos(angle));
        double new_pos_y = pos[1] + (distance_per_cycle_ * std::sin(angle));
        vehicle.SetPosition(new_pos_x, new_pos_y);
    }
}

void VehicleManager::Simulate() {
    // Launch Drive function in a thread
    threads.emplace_back(std::thread(&VehicleManager::Drive, this));
}

void VehicleManager::Drive() {
    // Create the route planner to use throughout the sim
    RoutePlanner route_planner = RoutePlanner(*model_);

    while (true) {
        // Sleep at every iteration to reduce CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        for (auto &vehicle : vehicles_) {
            // Get a route if none yet given
            if (vehicle.Path().empty()) {
                route_planner.AStarSearch(vehicle);
                // TODO: Replace/remove below when handling impossible routes (i.e. stuck)
                // TODO: Handle below if holding a passenger
                if (vehicle.Path().empty()) {
                    ResetVehicleDestination(vehicle, true);
                    continue;
                }
            }

            // Request a passenger if don't have one yet
            if (vehicle.State() == VehicleState::no_passenger_requested) {
                vehicle.SetState(VehicleState::no_passenger_queued);
                // Request a passenger
                RequestPassenger(vehicle.Id());
            }

            // Drive to destination or wait, depending on state
            if (vehicle.State() == VehicleState::waiting) {
                continue;
            } else {
                // Drive to current destination (effectively random driving)
                IncrementalMove(vehicle);
            }

            // Check if at destination
            // TODO: Ensure position and destination ensured to actually match?
            if (vehicle.GetPosition() == vehicle.GetDestination()) {
                if (vehicle.State() == VehicleState::no_passenger_queued) {
                    // Find a new random destination
                    ResetVehicleDestination(vehicle, true);
                } else if (vehicle.State() == VehicleState::passenger_queued) {
                    // Notify of arrival
                    ArrivedAtPassenger(vehicle.Id());
                    // Transition to waiting
                    vehicle.SetState(VehicleState::waiting);
                    // TODO: Output notice to console?
                } else if (vehicle.State() == VehicleState::driving_passenger) {
                    // Drop-off passenger
                    vehicle.DropOffPassenger();
                    // Transition back to no passenger requested state
                    vehicle.SetState(VehicleState::no_passenger_requested);
                    // Find a new random destination
                    ResetVehicleDestination(vehicle, true);
                    // TODO: Output notice to console?
                }
            }
        }
    }
}

void VehicleManager::RequestPassenger(int id) {
    if (ride_matcher_ != nullptr) {
        ride_matcher_->VehicleRequestsPassenger(id);
    }
}

void VehicleManager::AssignPassenger(int id, std::vector<double> position) {
    Vehicle vehicle = vehicles_.at(id);
    // Set new vehicle destination and update its state
    vehicle.SetDestination(position[0], position[1]);
    ResetVehicleDestination(vehicle, false); // Aligns to route node and resets path and index
    vehicle.SetState(VehicleState::passenger_queued);
}

void VehicleManager::ArrivedAtPassenger(int id) {
    ride_matcher_->VehicleHasArrived(id);
}

void VehicleManager::PassengerIntoVehicle(int id, std::shared_ptr<Passenger> passenger) {
    Vehicle vehicle = vehicles_.at(id);
    // Set passenger into vehicle and updates its state
    vehicle.SetPassenger(passenger); // Vehicle handles setting new destination with passenger
    ResetVehicleDestination(vehicle, false); // Aligns to route node and resets path and index
    vehicle.SetState(VehicleState::driving_passenger);
}