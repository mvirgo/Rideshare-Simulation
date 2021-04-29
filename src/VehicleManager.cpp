#include "VehicleManager.h"
#include <cmath>
#include "RoutePlanner.h"

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

void VehicleManager::ResetVehicleDestination(Vehicle &vehicle) {
    // TODO: Refactor below if needed to handle state transitions too
    auto destination = model_->GetRandomMapPosition();
    auto nearest_dest = model_->FindClosestNode(destination[0], destination[1]);
    vehicle.SetDestination(nearest_dest.x, nearest_dest.y);
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
                if (vehicle.Path().empty()) {
                    ResetVehicleDestination(vehicle);
                    continue;
                }
            }

            // Process action based on state
            if (vehicle.State() == VehicleState::no_passenger_queued) {
                // TODO: Try to request a passenger
                // Drive to current destination (effectively random driving)
                IncrementalMove(vehicle);
            } else if (vehicle.State() == VehicleState::passenger_queued) {
                // TODO: Drive to passenger position
            } else { // Have passenger in vehicle
                // TODO: Drive to passenger destination
            }

            // Check if at destination
            // TODO: Ensure position and destination ensured to actually match?
            if (vehicle.GetPosition() == vehicle.GetDestination()) {
                // TODO: Add handling of passenger (pick-up and drop-off)
                // TODO: Handle state transitions?
                // Reset vehicle with new random dest and no passenger queued
                // TODO: Refactor below when above TODO also implemented
                ResetVehicleDestination(vehicle);
            }
        }
    }
}