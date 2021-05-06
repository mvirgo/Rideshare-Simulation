#ifndef VEHICLEMANAGER_H
#define VEHICLEMANAGER_H

#include <unordered_map>
#include <vector>

#include "ConcurrentObject.h"
#include "Coordinate.h"
#include "ObjectHolder.h"
#include "Passenger.h"
#include "RouteModel.h"
#include "RoutePlanner.h"
#include "Vehicle.h"

// Avoid circular includes
namespace rideshare {
    class RideMatcher;
}

namespace rideshare {

class VehicleManager : public ConcurrentObject, public ObjectHolder, std::enable_shared_from_this<VehicleManager> {
  public:
    // Constructor / Destructor
    VehicleManager() {};
    VehicleManager(RouteModel *model, std::shared_ptr<RoutePlanner> route_planner);
    
    // Getters / Setters
    const std::unordered_map<int, std::shared_ptr<Vehicle>>& Vehicles() { return vehicles_; }
    void SetRideMatcher(std::shared_ptr<RideMatcher> ride_matcher) { ride_matcher_ = ride_matcher; }

    void Simulate();

    // Passenger-related handling
    void AssignPassenger(int id, Coordinate position);
    void PassengerIntoVehicle(int id, std::shared_ptr<Passenger> passenger);

    // Miscellaneous
    std::shared_ptr<VehicleManager> GetSharedThis() { return shared_from_this(); }

  private:
    // Creation
    void GenerateNew();

    // Movement
    void Drive();
    void IncrementalMove(std::shared_ptr<Vehicle> vehicle);
    void ResetVehicleDestination(std::shared_ptr<Vehicle> vehicle, bool random);
    void SimpleVehicleFailure(std::shared_ptr<Vehicle> vehicle);

    // Passenger-related handling
    void RequestPassenger(std::shared_ptr<Vehicle> vehicle);
    void ArrivedAtPassenger(std::shared_ptr<Vehicle> vehicle);
    void DropOffPassenger(std::shared_ptr<Vehicle> vehicle);

    // Variables
    std::unordered_map<int, std::shared_ptr<Vehicle>> vehicles_;
    std::vector<int> to_remove_;
    double distance_per_cycle_;
    std::shared_ptr<RideMatcher> ride_matcher_;
};

}  // namespace rideshare

#endif
