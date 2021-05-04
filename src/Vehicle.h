#ifndef VEHICLE_H
#define VEHICLE_H

#include <memory>
#include <vector>

#include "Coordinate.h"
#include "MapObject.h"
#include "RouteModel.h"

class Passenger;

enum VehicleState {
    no_passenger_requested,
    no_passenger_queued,
    passenger_queued,
    waiting,
    driving_passenger,
};

class Vehicle: public MapObject {
  public:
    // Constructor / Destructor

    // Getters / Setters
    int Shape() { return shape_; }
    int State() { return state_; }
    int PathIndex() { return path_index_; }
    std::shared_ptr<Passenger> GetPassenger() { return passenger_; }
    std::vector<Model::Node> Path() { return path_; }
    void SetState(VehicleState state) { state_ = state; }
    void SetPath(std::vector<Model::Node> path) { path_ = path; }
    void SetPassenger(std::shared_ptr<Passenger> passenger);
    void SetPosition(const Coordinate &position); // Override base class

    // Other functionality
    void DropOffPassenger() { passenger_.reset(); } // TODO: May want more post-dropoff later
    void IncrementPathIndex() { ++path_index_; }
    void ResetPathAndIndex();

  private:
    std::shared_ptr<Passenger> passenger_;
    int shape_ = DrawMarker::square;
    int state_ = VehicleState::no_passenger_requested;
    int path_index_ = 0;
    std::vector<Model::Node> path_;
};

#endif