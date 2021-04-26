#ifndef PASSENGER_H
#define PASSENGER_H

#include "MapObject.h"

class Passenger: public MapObject {
  public:
    // constructor / destructor
    Passenger(float start_x, float start_y, float dest_x, float dest_y);

    // getters
    std::vector<float> GetDestination() { return {dest_x_, dest_y_}; }

  private:
    float dest_x_, dest_y_;
};

#endif