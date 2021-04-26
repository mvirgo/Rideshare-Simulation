#ifndef VEHICLEMANAGER_H
#define VEHICLEMANAGER_H

#include <vector>
#include "Vehicle.h"
#include "RouteModel.h"

// TODO: Change this to run concurrently so can simulate adding more
class VehicleManager {
  public:
    // constructor / destructor
    VehicleManager();
    VehicleManager(RouteModel *model);
    
    // getter
    std::vector<Vehicle> Vehicles() { return vehicles_; }

  private:
    void GenerateVehicle();
    void Simulate();
    int MAX_VEHICLES = 10; // TODO: Change or remove limit?
    RouteModel *model_;
    std::vector<Vehicle> vehicles_;
};

#endif