#ifndef MAPOBJECT_H
#define MAPOBJECT_H

#include <cstdlib>
#include <vector>

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
    void SetPosition(double x, double y) { x_ = x; y_ = y; }
    void SetDestination(double dest_x, double dest_y) { dest_x_ = dest_x; dest_y_ = dest_y; }
    void SetColors(int blue, int green, int red) { blue_ = blue; green_ = green; red_ = red; }
    void SetId(int id) { id_ = id; }
    std::vector<double> GetPosition() { return {x_, y_}; }
    std::vector<double> GetDestination() { return {dest_x_, dest_y_}; }
    int Blue() { return blue_; }
    int Green() { return green_; }
    int Red() { return red_; }
    int Id() { return id_; }

  protected:
    // member variables
    int id_;
    double x_, y_;
    double dest_x_, dest_y_;
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