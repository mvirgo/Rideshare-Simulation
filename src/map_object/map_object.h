/**
 * @file map_object.h
 * @brief Parent for classes drawn onto the map.
 *
 * @copyright Copyright (c) 2021, Michael Virgo, released under the MIT License.
 *
 */

#ifndef MAP_OBJECT_H_
#define MAP_OBJECT_H_

#include <cmath>
#include <cstdlib>
#include <vector>

#include "mapping/coordinate.h"
#include "mapping/model.h"

namespace rideshare {

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
    // Constructor / Destructor
    MapObject(double distance_per_cycle) : distance_per_cycle_(distance_per_cycle) {
      SetRandomColors();
    }

    // Getters / Setters
    void SetPosition(const Coordinate &position) { position_ = position; }
    void SetDestination(const Coordinate &destination) { destination_ = destination; }
    void SetColors(int blue, int green, int red) { blue_ = blue; green_ = green; red_ = red; }
    void SetId(int id) { id_ = id; }
    void SetPath(std::vector<Model::Node> path) { path_ = path; }
    Coordinate GetPosition() { return position_; }
    Coordinate GetDestination() { return destination_; }
    int Blue() { return blue_; }
    int Green() { return green_; }
    int Red() { return red_; }
    int Id() { return id_; }
    std::vector<Model::Node> Path() { return path_; }

    // Movement
    virtual void IncrementalMove() {};

    // Handling of failures (such as destination can't be reached from position)
    bool MovementFailure() {
        ++failures_;
        return failures_ >= MAX_FAILURES_;
    }

  protected:
    // Get an intermediate position between current position and desired next position
    Coordinate GetIntermediatePosition(double next_x, double next_y) {
        double angle = std::atan2(next_y - position_.y, next_x - position_.x); // angle from x-axis
        double new_pos_x = position_.x + (distance_per_cycle_ * std::cos(angle));
        double new_pos_y = position_.y + (distance_per_cycle_ * std::sin(angle));
        return (Coordinate){.x = new_pos_x, .y = new_pos_y};
    }

    // Member variables
    int id_;
    int failures_ = 0;
    const double distance_per_cycle_; // max distance to move per cycle for smooth-looking movement
    int MAX_FAILURES_ = 10; // max failures before object will be removed (likely stuck)
    Coordinate position_;
    Coordinate destination_;
    int blue_, green_, red_; // Visualization colors
    std::vector<Model::Node> path_; // path made by route planner from start position to destination
  
  private:
    // Set visualization colors out of 255
    void SetRandomColors() {
        blue_ = (int)(((float) rand() / RAND_MAX) * 255);
        green_ = (int)(((float) rand() / RAND_MAX) * 255);
        red_ = (int)(((float) rand() / RAND_MAX) * 255);
    }
};

}  // namespace rideshare

#endif  // MAP_OBJECT_H_
