/**
 * @file vehicle_manager.h
 * @brief Handles generation of and driving of vehicles around the map.
 *
 * @copyright Copyright (c) 2021, Michael Virgo, released under the MIT License.
 *
 */

#ifndef VEHICLE_MANAGER_H_
#define VEHICLE_MANAGER_H_

#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>

#include "concurrent_object.h"
#include "object_holder.h"
#include "mapping/coordinate.h"
#include "mapping/route_model.h"
#include "map_object/passenger.h"
#include "map_object/vehicle.h"
#include "routing/route_planner.h"

// Avoid circular includes
namespace rideshare {
    class RideMatcher;
}

namespace rideshare {

class VehicleManager : public ConcurrentObject, public ObjectHolder {
  public:
    // Constructor / Destructor
    VehicleManager(RouteModel *model, std::shared_ptr<RoutePlanner> route_planner, int max_objects);
    
    // Getters / Setters
    const std::unordered_map<int, std::shared_ptr<Vehicle>>& Vehicles() { return vehicles_; }
    void SetRideMatcher(std::shared_ptr<RideMatcher> ride_matcher) { ride_matcher_ = ride_matcher; }

    // Concurrent simulation
    void Simulate();

    // Passenger-related handling
    // Receive any new passenger assignments
    void AssignPassenger(int id, Coordinate position);
    // Receive any passengers ready to be picked up by specified vehicle its post-arrival
    void PassengerIntoVehicle(int id, std::shared_ptr<Passenger> passenger);

  private:
    // Creation
    void GenerateNew();

    // Movement
    // Handle loop cycle of movements and actions based on assignments / arrival at passengers
    void Drive();
    // Either gets a random map position, or uses the given destination, and aligns either to closest map node
    void ResetVehicleDestination(std::shared_ptr<Vehicle> vehicle, bool random);
    // Vehicle has encountered some type of issue reaching a given destination, without a passenger within
    void SimpleVehicleFailure(std::shared_ptr<Vehicle> vehicle);

    // Passenger-related handling
    // Request a passenger to pick up from the ride matcher
    void RequestPassenger(std::shared_ptr<Vehicle> vehicle);
    // Notify specified vehicle of passenger assignment, given the passenger's current position
    void NewPassengerAssignments();
    // Handle aspects of being unable to reach a matched passenger (notify ride matcher, re-request, add a simple failure)
    void AssignmentFailure(std::shared_ptr<Vehicle> vehicle);
    // Notify ride matcher that vehicle arrived at node nearest to passenger position
    void ArrivedAtPassenger(std::shared_ptr<Vehicle> vehicle);
    // Pick up any passengers now ready to be picked up post-arrival
    void PickUpPassengers();
    // Drop off the passenger once nearest node to its destination is reached (remove from vehicle)
    void DropOffPassenger(std::shared_ptr<Vehicle> vehicle);

    // Variables
    std::unordered_map<int, std::shared_ptr<Vehicle>> vehicles_;
    std::unordered_map<int, std::shared_ptr<Passenger>> passenger_pickups_; // store passenger pickups for next cycle
    std::unordered_map<int, Coordinate> new_assignment_locations; // store new assignments for next cycle
    std::vector<int> to_remove_; // store vehicle ids of those to remove the next cycle (due to too many failures)
    std::shared_ptr<RideMatcher> ride_matcher_;
    std::mutex passenger_pickups_mutex; // protect read/write access to passenger pickups between cycles
    std::mutex new_assignment_locations_mutex; // protect read/write access to new assignments between cycles
};

}  // namespace rideshare

#endif // VEHICLE_MANAGER_H_
