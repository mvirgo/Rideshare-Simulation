/**
 * @file passenger_queue.h
 * @brief Handles generation of passengers and their behavior prior to pick-up by vehicles.
 *
 * @copyright Copyright (c) 2021, Michael Virgo, released under the MIT License.
 *
 */

#ifndef PASSENGER_QUEUE_H_
#define PASSENGER_QUEUE_H_

#include <memory>
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

class PassengerQueue : public ConcurrentObject, public ObjectHolder, public MessageHandler {
  public:
    // Messages are sent with below enum code and passenger id
    enum MsgCodes {
        ride_on_way,
        ride_arrived,
        passenger_picked_up,
        passenger_failure,
    };

    // Constructor / Destructor
    PassengerQueue(RouteModel *model, std::shared_ptr<RoutePlanner> route_planner,
                   int max_objects, int min_wait_time, int range_wait_time);
    
    // Getters / Setters
    const std::unordered_map<int, std::shared_ptr<Passenger>>& NewPassengers() { return new_passengers_; }
    const std::unordered_map<int, std::shared_ptr<Passenger>>& WalkingPassengers() { return walking_passengers_; }
    void SetRideMatcher(std::shared_ptr<RideMatcher> ride_matcher) { ride_matcher_ = ride_matcher; }

    // Concurrent simulation
    void Simulate();

    // Message receiving
    void Message(SimpleMessage simple_message);

  private:
    // Creation
    // Regularly generate more passengers
    void GenerateNew();
    // Handles loop cycle of generation, reading messages, requesting rides
    void WaitForRide();

    // Ride match handling
    // Request a ride for a given passenger
    void RequestRide(std::shared_ptr<Passenger> passenger);
    // Notification that ride is on the way for a passenger
    void RideOnWay(int id);
    // Notification that ride has arrived for a passenger
    void RideArrived(int id);
    // Passenger has walked to and arrived at the vehicle
    void PassengerAtVehicle(int id);
    // Notification that a passenger was picked up by vehicle, and can be removed locally
    void PassengerPickedUp(int id);

    // Passenger walking to vehicle functionality
    void WalkPassengersToVehicles();

    // Message reading - take action based on given message
    void ReadMessages();

    // Failure handling
    void PassengerFailure(int id);

    // Variables
    const int MIN_WAIT_TIME_; // seconds to wait between generation attempts
    const int RANGE_WAIT_TIME_; // range in seconds to wait between generation attempts
    std::unordered_map<int, std::shared_ptr<Passenger>> new_passengers_;
    std::unordered_map<int, std::shared_ptr<Passenger>> walking_passengers_;
    std::shared_ptr<RideMatcher> ride_matcher_;
};

}  // namespace rideshare

#endif  // PASSENGER_QUEUE_H_
