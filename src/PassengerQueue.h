#ifndef PASSENGER_QUEUE_H
#define PASSENGER_QUEUE_H

#include <unordered_map>
#include <vector>
#include "ConcurrentObject.h"

#include "Passenger.h"
#include "RouteModel.h"
#include "RoutePlanner.h"

class RideMatcher;

class PassengerQueue : public ConcurrentObject, std::enable_shared_from_this<PassengerQueue> {
  public:
    // Constructor / Destructor
    PassengerQueue() {};
    PassengerQueue(RouteModel *model, std::shared_ptr<RoutePlanner> route_planner);
    
    // Getters / Setters
    const std::unordered_map<int, std::shared_ptr<Passenger>>& NewPassengers() { return new_passengers_; }
    void SetRideMatcher(std::shared_ptr<RideMatcher> ride_matcher) { ride_matcher_ = ride_matcher; }

    // Concurrency
    void Simulate();

    // Ride match handling
    void RideOnWay(int id);
    void RideArrived(int id);

    // Failure handling
    void PassengerFailure(int id);

    // Miscellaneous
    std::shared_ptr<PassengerQueue> GetSharedThis() { return shared_from_this(); }

  private:
    // Creation
    void GenerateNew();
    void WaitForRide();

    // Ride match handling
    void RequestRide(std::shared_ptr<Passenger> passenger);

    // Variables
    int MIN_WAIT_TIME_ = 3; // seconds to wait between generation attempts
    int RANGE_WAIT_TIME_ = 2; // range in seconds to wait between generation attempts
    std::unordered_map<int, std::shared_ptr<Passenger>> new_passengers_;
    std::shared_ptr<RideMatcher> ride_matcher_;
};

#endif