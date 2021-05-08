/**
 * @file passenger_queue.h
 * @brief Handles generation of passengers and their behavior prior to pick-up by vehicles.
 *
 * @copyright Copyright (c) 2021, Michael Virgo, released under the MIT License.
 *
 */

#ifndef PASSENGER_QUEUE_H_
#define PASSENGER_QUEUE_H_

#include <mutex>
#include <unordered_map>
#include <vector>

#include "concurrent_object.h"
#include "message_handler.h"
#include "object_holder.h"
#include "simple_message.h"
#include "mapping/route_model.h"
#include "map_object/passenger.h"
#include "routing/route_planner.h"

// Avoid circular includes
namespace rideshare {
    class RideMatcher;
}

namespace rideshare {

class PassengerQueue : public ConcurrentObject, public ObjectHolder, public MessageHandler, std::enable_shared_from_this<PassengerQueue> {
  public:
    // Messages are sent with below enum code and passenger id
    enum MsgCodes {
        ride_on_way,
        ride_arrived,
        passenger_failure,
    };

    // Constructor / Destructor
    PassengerQueue() {};
    PassengerQueue(RouteModel *model, std::shared_ptr<RoutePlanner> route_planner);
    
    // Getters / Setters
    const std::unordered_map<int, std::shared_ptr<Passenger>>& NewPassengers() { return new_passengers_; }
    void SetRideMatcher(std::shared_ptr<RideMatcher> ride_matcher) { ride_matcher_ = ride_matcher; }

    // Concurrency
    void Simulate();

    // Message receiving
    void Message(SimpleMessage simple_message);

    // Miscellaneous
    std::shared_ptr<PassengerQueue> GetSharedThis() { return shared_from_this(); }

  private:
    // Creation
    void GenerateNew();
    void WaitForRide();

    // Ride match handling
    void RequestRide(std::shared_ptr<Passenger> passenger);
    void RideOnWay(int id);
    void RideArrived(int id);

    // Message reading
    void ReadMessages();

    // Failure handling
    void PassengerFailure(int id);

    // Variables
    int MIN_WAIT_TIME_ = 3; // seconds to wait between generation attempts
    int RANGE_WAIT_TIME_ = 2; // range in seconds to wait between generation attempts
    std::unordered_map<int, std::shared_ptr<Passenger>> new_passengers_;
    std::shared_ptr<RideMatcher> ride_matcher_;
};

}  // namespace rideshare

#endif  // PASSENGER_QUEUE_H_
