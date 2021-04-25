#ifndef BASICINTERSECTION_H
#define BASICINTERSECTION_H

#include <vector>

class BasicIntersection
{
public:
    // constructor / destructor

    // getters / setters
    void setPosition(float x, float y) { _x = x; _y = y;}
    std::vector<float> getPosition() { return {_x, _y}; }

private:
    // member variables
    float _x, _y;
};

#endif