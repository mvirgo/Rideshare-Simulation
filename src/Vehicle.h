#ifndef VEHICLE_H
#define VEHICLE_H

#include <memory>
#include <vector>
#include "MapObject.h"
#include "RouteModel.h"

class Passenger;

enum VehicleState {
    no_passenger_queued,
    passenger_queued,
    driving_passenger,
};

class Vehicle: public MapObject {
  public:
    // constructor / destructor

    // getters / setters
    int Shape() { return shape_; }
    int State() { return state_; }
    int PathIndex() { return path_index_; }
    void SetState(VehicleState state) { state_ = state; }
    std::vector<RouteModel::Node> Path() { return path_; }
    void SetPath(std::vector<RouteModel::Node> path) { path_ = path; }
    void IncrementPathIndex() { ++path_index_; }
    void ResetPathAndIndex() {
        path_.clear();
        path_index_ = 0;
    }

  private:
    std::shared_ptr<Passenger> passenger_;
    int shape_ = DrawMarker::square;
    int state_ = VehicleState::no_passenger_queued;
    int path_index_ = 0;
    std::vector<RouteModel::Node> path_;
    // TODO: Functions for sim/driving and handling passenger requests
};

#endif