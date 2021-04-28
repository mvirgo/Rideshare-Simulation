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
    // Find the nearest road node to start position
    auto nearest = model_->FindClosestNode(start[0], start[1]);
    // Set road position and id of vehicle
    Vehicle vehicle = Vehicle(); // TODO: May need change for memory changes later
    vehicle.SetPosition(nearest.x, nearest.y);
    vehicle.SetId(idCnt_++);
    vehicles_.emplace_back(vehicle);
    // Output id and location of vehicle looking to give rides
    std::lock_guard<std::mutex> lck(mtx_);
    std::cout << "Vehicle ID#" << idCnt_ - 1 << " now driving from: " << nearest.y << ", " << nearest.x << "." << std::endl;
}

void VehicleManager::Simulate() {
    // TODO: Implement
}