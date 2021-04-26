#ifndef BASICINTERSECTION_H
#define BASICINTERSECTION_H

#include "MapObject.h"

class BasicIntersection : public MapObject {
  public:
    BasicIntersection() { blue_ = 0; green_ = 255; red_ = 0; }
};

#endif