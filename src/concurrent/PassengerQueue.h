#ifndef PASSENGER_QUEUE_H_
#define PASSENGER_QUEUE_H_

#include <unordered_map>
#include <vector>

#include "ConcurrentObject.h"
#include "ObjectHolder.h"
#include "mapping/RouteModel.h"
#include "map_object/Passenger.h"
#include "routing/RoutePlanner.h"

// Avoid circular includes
namespace rideshare {
    class RideMatcher;
}

namespace rideshare {

class PassengerQueue : public ConcurrentObject, public ObjectHolder, std::enable_shared_from_this<PassengerQueue> {
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

}  // namespace rideshare

#endif  // PASSENGER_QUEUE_H_
