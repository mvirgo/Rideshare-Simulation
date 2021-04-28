#ifndef VEHICLE_H
#define VEHICLE_H

#include <memory>
#include "MapObject.h"

class Passenger;

class Vehicle: public MapObject {
  public:
    // constructor / destructor

    // getters / setters

  private:
    std::shared_ptr<Passenger> passenger;
    // TODO: Functions for sim/driving and handling passenger requests
};

#endif