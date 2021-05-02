#ifndef VEHICLEMANAGER_H
#define VEHICLEMANAGER_H

#include <vector>
#include "ConcurrentObject.h"
#include "Vehicle.h"

// Avoid include cycle
class Passenger;
class RideMatcher;

class VehicleManager : public ConcurrentObject, std::enable_shared_from_this<VehicleManager> {
  public:
    // Constructor / Destructor
    VehicleManager() {};
    VehicleManager(RouteModel *model);
    
    // Getters / Setters
    std::vector<Vehicle> Vehicles() { return vehicles_; }
    void SetRideMatcher(std::shared_ptr<RideMatcher> ride_matcher) { ride_matcher_ = ride_matcher; }

    void Simulate();

    // Passenger-related handling
    void AssignPassenger(int id, std::vector<double> position);
    void PassengerIntoVehicle(int id, std::shared_ptr<Passenger> passenger);

    // Miscellaneous
    std::shared_ptr<VehicleManager> GetSharedThis() { return shared_from_this(); }

  private:
    // Creation
    void GenerateNew();

    // Movement
    void Drive();
    void IncrementalMove(Vehicle &vehicle);
    void ResetVehicleDestination(Vehicle &vehicle, bool random);

    // Passenger-related handling
    void RequestPassenger(int id);
    void ArrivedAtPassenger(int id);

    // Variables
    std::vector<Vehicle> vehicles_;
    double distance_per_cycle_;
    std::shared_ptr<RideMatcher> ride_matcher_;
};

#endif