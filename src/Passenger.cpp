#include "Passenger.h"
#include <cstdlib>

Passenger::Passenger(float start_x, float start_y, float dest_x, float dest_y) {
    x_ = start_x;
    y_ = start_y;
    dest_x_ = dest_x;
    dest_y_ = dest_y;

    SetRandomColors();
}