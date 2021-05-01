#ifndef VEHICLEMANAGER_H
#define VEHICLEMANAGER_H

#include <vector>
#include "ConcurrentObject.h"
#include "Vehicle.h"

class VehicleManager : public ConcurrentObject, std::enable_shared_from_this<VehicleManager> {
  public:
    // constructor / destructor
    VehicleManager() {};
    VehicleManager(RouteModel *model);
    
    // getter
    std::vector<Vehicle> Vehicles() { return vehicles_; }

    void Simulate();

    // miscellaneous
    std::shared_ptr<VehicleManager> GetSharedThis() { return shared_from_this(); }

  private:
    void Drive();
    void IncrementalMove(Vehicle &vehicle);
    void GenerateNew();
    void ResetVehicleDestination(Vehicle &vehicle);
    std::vector<Vehicle> vehicles_;
    double distance_per_cycle_;
};

#endif