/**
 * @file map_object.h
 * @brief Parent for classes drawn onto the map.
 *
 * @copyright Copyright (c) 2021, Michael Virgo, released under the MIT License.
 *
 */

#ifndef MAP_OBJECT_H_
#define MAP_OBJECT_H_

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
    MapObject() { SetRandomColors(); }

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

    // Handling of failures (such as destination can't be reached from position)
    bool MovementFailure() {
        ++failures_;
        return failures_ >= MAX_FAILURES_;
    }

  protected:
    // Member variables
    int id_;
    int failures_ = 0;
    int MAX_FAILURES_ = 10;
    Coordinate position_;
    Coordinate destination_;
    int blue_, green_, red_; // Visualization colors
    std::vector<Model::Node> path_;
  
  private:
    void SetRandomColors() {
        // Set visualization colors out of 255
        blue_ = (int)(((float) rand() / RAND_MAX) * 255);
        green_ = (int)(((float) rand() / RAND_MAX) * 255);
        red_ = (int)(((float) rand() / RAND_MAX) * 255);
    }
};

}  // namespace rideshare

#endif  // MAP_OBJECT_H_
