#ifndef MAPOBJECT_H
#define MAPOBJECT_H

#include <cstdlib>
#include <vector>

#include "Coordinate.h"

enum DrawMarker {
    // Values for shapes for cv::drawMarker
    cross,
    tilted_cross,
    star,
    diamond,
    square,
    triangle_up,
    triangle_down,
};

class MapObject {
  public:
    // constructor / destructor
    MapObject() { SetRandomColors(); }

    // getters / setters
    void SetPosition(const Coordinate &position) { position_ = position; }
    void SetDestination(const Coordinate &destination) { destination_ = destination; }
    void SetColors(int blue, int green, int red) { blue_ = blue; green_ = green; red_ = red; }
    void SetId(int id) { id_ = id; }
    Coordinate GetPosition() { return position_; }
    Coordinate GetDestination() { return destination_; }
    int Blue() { return blue_; }
    int Green() { return green_; }
    int Red() { return red_; }
    int Id() { return id_; }

  protected:
    // member variables
    int id_;
    Coordinate position_;
    Coordinate destination_;
    int blue_, green_, red_; // visualization colors
  
  private:
    void SetRandomColors() {
        // Set visualization colors out of 255
        blue_ = (int)(((float) rand() / RAND_MAX) * 255);
        green_ = (int)(((float) rand() / RAND_MAX) * 255);
        red_ = (int)(((float) rand() / RAND_MAX) * 255);
    }
};

#endif