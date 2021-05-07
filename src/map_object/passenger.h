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
    // constructor / destructor

    // getters / setters
    int PassShape() { return pass_shape_; }
    int DestShape() { return dest_shape_; }
    bool RideRequested() { return ride_requested_; }
    void SetRideRequest(bool requested) { ride_requested_ = requested; }
  
  private:
    int pass_shape_ = DrawMarker::diamond;
    int dest_shape_ = DrawMarker::tilted_cross;
    bool ride_requested_ = false;
};

}  // namespace rideshare

#endif  // PASSENGER_H_
