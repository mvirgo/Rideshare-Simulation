#include "VehicleManager.h"

VehicleManager::VehicleManager(RouteModel *model) : ConcurrentObject(model) {
    // TODO: Add simulation instead of creating vehicles at start?
    for (int i = 0; i < MAX_OBJECTS; ++i) {
        GenerateNew();
    }
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

void VehicleManager::Simulate() {
    // TODO: Implement
}