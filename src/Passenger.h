#ifndef PASSENGER_H
#define PASSENGER_H

class Passenger
{
  public:
    // constructor / destructor
    Passenger(float start_x, float start_y, float dest_x, float dest_y);

    // getters
    float startX() { return start_x_; }
    float startY() { return start_y_; }
    float destX() { return dest_x_; }
    float destY() { return dest_y_; }
    int Blue() { return blue_; }
    int Green() { return green_; }
    int Red() { return red_; }

  private:
    float start_x_, start_y_, dest_x_, dest_y_;
    int blue_, green_, red_; // visualization colors
};

#endif