#ifndef VEHICLE_H
#define VEHICLE_H

#include <memory>
#include <vector>
#include "MapObject.h"
#include "RouteModel.h"

class Passenger;

class Vehicle: public MapObject {
  public:
    // constructor / destructor

    // getters / setters
    int Shape() { return shape_; }
    std::vector<RouteModel::Node> Path() { return path_; }
    void SetPath(std::vector<RouteModel::Node> path) { path_ = path; }

  private:
    std::shared_ptr<Passenger> passenger_;
    int shape_ = DrawMarker::square;
    std::vector<RouteModel::Node> path_;
    // TODO: Functions for sim/driving and handling passenger requests
};

#endif