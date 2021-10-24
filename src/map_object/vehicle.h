/**
 * @file vehicle.h
 * @brief Vehicle to request, pick up and drop off passengers; shown on the map.
 *
 * @copyright Copyright (c) 2021, Michael Virgo, released under the MIT License.
 *
 */

#ifndef VEHICLE_H_
#define VEHICLE_H_

#include <memory>
#include <mutex>
#include <vector>

#include "map_object.h"
#include "mapping/coordinate.h"
#include "mapping/route_model.h"
#include "map_object/passenger.h"

namespace rideshare {

enum VehicleState {
    no_passenger_requested,
    no_passenger_queued,
    passenger_queued,
    waiting,
    driving_passenger,
};

class Vehicle: public MapObject {
  public:
    // Constructor / Destructor
    Vehicle(double distance_per_cycle) : MapObject(distance_per_cycle) {}

    // Getters / Setters
    int Shape() { return shape_; }
    int State() { return state_; }
    int PathIndex() { return path_index_; }
    std::shared_ptr<Passenger> GetPassenger() { return passenger_; }
    void SetState(VehicleState state) { state_ = state; }
    void SetPassenger(std::shared_ptr<Passenger> passenger);
    // Override base class - also set passenger position if there is one to match vehicle
    void SetPosition(const Coordinate &position);
    // Override base class - use ResetPathAndIndex within so will get a new path and increment properly
    void SetDestination(const Coordinate &destination);

    // Other functionality
    // "Drop off" the passenger - remove the passenger and reset any failures
    void DropOffPassenger();
    // Movement
    void IncrementalMove();
    // Increment path index by 1
    void IncrementPathIndex() { ++path_index_; }

  private:
    // Clear the path and reset path index to zero so can increment along the path properly
    void ResetPathAndIndex();

    std::shared_ptr<Passenger> passenger_;
    int shape_ = DrawMarker::square;
    int state_ = VehicleState::no_passenger_requested;
    int path_index_ = 0;
};

}  // namespace rideshare

#endif  // VEHICLE_H_
