#ifndef VEHICLEMANAGER_H
#define VEHICLEMANAGER_H

#include <vector>
#include "ConcurrentObject.h"
#include "Vehicle.h"

// TODO: Change this to run concurrently so can simulate adding more
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
    void GenerateNew();
    std::vector<Vehicle> vehicles_;
};

#endif