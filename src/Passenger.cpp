#include "Passenger.h"
#include <cstdlib>

Passenger::Passenger(float start_x, float start_y, float dest_x, float dest_y) {
    start_x_ = start_x;
    start_y_ = start_y;
    dest_x_ = dest_x;
    dest_y_ = dest_y;

    // Set visualization colors out of 255
    blue_ = (int)(((float) rand() / RAND_MAX) * 255);
    green_ = (int)(((float) rand() / RAND_MAX) * 255);
    red_ = (int)(((float) rand() / RAND_MAX) * 255);
}