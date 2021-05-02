#ifndef PASSENGER_H
#define PASSENGER_H

#include "MapObject.h"

class Passenger: public MapObject {
  public:
    // constructor / destructor

    // getters / setters
    int PassShape() { return pass_shape_; }
    int DestShape() { return dest_shape_; }
    bool RideRequested() { return ride_requested_; }
    void SetRideAsRequested() { ride_requested_ = true; }
  
  private:
    int pass_shape_ = DrawMarker::diamond;
    int dest_shape_ = DrawMarker::tilted_cross;
    bool ride_requested_ = false;
};

#endif