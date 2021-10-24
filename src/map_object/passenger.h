/**
 * @file passenger.h
 * @brief Passengers to request and take rides; shown on the map.
 *
 * @copyright Copyright (c) 2021, Michael Virgo, released under the MIT License.
 *
 */

#ifndef PASSENGER_H_
#define PASSENGER_H_

#include "map_object.h"

namespace rideshare {

class Passenger: public MapObject {
  public:
    // Constructor / Destructor
    Passenger(double distance_per_cycle) : MapObject(distance_per_cycle) {}

    // Enum for statuses
    enum PassengerStatus {
      no_ride_requested,
      ride_requested,
      walking,
      at_ride,
    };

    // Getters / Setters
    int PassShape() { return pass_shape_; }
    int DestShape() { return dest_shape_; }
    int GetStatus() { return status_; }
    void SetStatus(int status) { status_ = status; }
    void SetWalkToPos(Model::Node& walk_to_pos) { walk_to_pos_ = walk_to_pos; }

    // Movement
    void IncrementalMove();
  
  private:
    int pass_shape_ = DrawMarker::diamond;
    int dest_shape_ = DrawMarker::tilted_cross;
    int status_ = PassengerStatus::no_ride_requested;
    Model::Node walk_to_pos_;
};

}  // namespace rideshare

#endif  // PASSENGER_H_
