#ifndef MAPOBJECT_H
#define MAPOBJECT_H

#include <vector>

class MapObject {
  public:
    // constructor / destructor

    // getters / setters
    void SetPosition(float x, float y) { x_ = x; y_ = y; }
    std::vector<float> GetPosition() { return {x_, y_}; }
    void setColors(int blue, int green, int red) { blue_ = blue, green_ = green, red_ = red; }
    int Blue() { return blue_; }
    int Green() { return green_; }
    int Red() { return red_; }

  protected:
    // member variables
    float x_, y_;
    int blue_, green_, red_; // visualization colors
};

#endif