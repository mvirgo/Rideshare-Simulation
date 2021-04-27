#include "VehicleManager.h"

VehicleManager::VehicleManager(RouteModel *model) : ConcurrentObject(model) {
    // TODO: Add simulation instead of creating vehicles at start?
    for (int i = 0; i < MAX_OBJECTS; ++i) {
        GenerateNew();
    }
}

void VehicleManager::GenerateNew() {
    // TODO: Add appropriate handling of Vehicle to avoid memory leaks if made a pointer
    // TODO: Maybe add an id for vehicle so can grab desired one?
    auto start = model_->GetRandomMapPosition();
    auto nearest = model_->FindClosestNode(start[0], start[1]); // Set on road
    Vehicle vehicle = Vehicle(); // TODO: May need change for memory changes later
    vehicle.SetPosition(nearest.x, nearest.y);
    vehicles_.emplace_back(vehicle);
}

void VehicleManager::Simulate() {
    // TODO: Implement
}