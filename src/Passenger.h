#ifndef PASSENGER_H
#define PASSENGER_H

#include "MapObject.h"

class Passenger: public MapObject {
  public:
    // constructor / destructor
    Passenger();

    // getters
    void SetDestination(float dest_x, float dest_y) { dest_x_ = dest_x; dest_y_ = dest_y; }
    std::vector<float> GetDestination() { return {dest_x_, dest_y_}; }

  private:
    float dest_x_, dest_y_;
};

#endif