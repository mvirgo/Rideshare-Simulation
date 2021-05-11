/**
 * @file passenger_queue.cpp
 * @brief Implementation of queue for passengers pre-pickup by vehicles.
 *
 * @copyright Copyright (c) 2021, Michael Virgo, released under the MIT License.
 *
 */

#include "passenger_queue.h"

#include <chrono>
#include <mutex>

#include "ride_matcher.h"
#include "simple_message.h"
#include "mapping/route_model.h"
#include "map_object/passenger.h"
#include "routing/route_planner.h"

namespace rideshare {

PassengerQueue::PassengerQueue(RouteModel *model, std::shared_ptr<RoutePlanner> route_planner) : ObjectHolder(model, route_planner) {
    // Start by creating half the max number of passengers
    // Note that the while loop avoids generating less if any invalid placements occur
    while (new_passengers_.size() < MAX_OBJECTS / 2) {
        GenerateNew();
    }
}

void PassengerQueue::GenerateNew() {
    // Get random start and destination locations
    auto start = model_->GetRandomMapPosition();
    auto dest = model_->GetRandomMapPosition();
    // Set those to passenger
    std::shared_ptr<Passenger> passenger = std::make_shared<Passenger>();
    passenger->SetPosition(start);
    passenger->SetDestination(dest);
    // Set path with route planner, and verify the path between them is valid/reachable
    route_planner_->AStarSearch(passenger);
    if (passenger->Path().empty()) {
        // No valid path, reset and return
        passenger.reset();
        std::lock_guard<std::mutex> lck(mtx_);
        std::cout << "A new passenger with an unreachable destination from their position left." << std::endl;
        return;
    }
    // Set id to the passenger
    passenger->SetId(idCnt_++);
    new_passengers_.emplace(passenger->Id(), passenger);
    // Output id and location of passenger requesting ride
    std::lock_guard<std::mutex> lck(mtx_);
    std::cout << "Passenger #" << idCnt_ - 1 << " requesting ride from: " << start.y << ", " << start.x << "." << std::endl;
}

void PassengerQueue::Simulate() {
    // Launch WaitForRide function in a thread
    threads.emplace_back(std::thread(&PassengerQueue::WaitForRide, this));
}

void PassengerQueue::WaitForRide() {
    // Set wait time between potentially generating new passengers
    double cycleDuration = ((((float) rand() / RAND_MAX) * RANGE_WAIT_TIME_) + MIN_WAIT_TIME_) * 1000; // duration of a single simulation cycle in ms
    std::chrono::time_point<std::chrono::system_clock> lastUpdate = std::chrono::system_clock::now();

    while (true) {
        // Sleep at every iteration to reduce CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        // Compute time difference to stop watch
        long timeSinceLastUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - lastUpdate).count();

        // Check if cycleDuration passed and if less than max passengers before creating a new one
        if ((timeSinceLastUpdate >= cycleDuration) && (new_passengers_.size() < MAX_OBJECTS)) {
            GenerateNew();
            // Get a new random time to wait before checking to add a new passenger
            cycleDuration = ((((float) rand() / RAND_MAX) * RANGE_WAIT_TIME_) + MIN_WAIT_TIME_) * 1000;
            // Reset stop watch
            lastUpdate = std::chrono::system_clock::now();
        } else if ((timeSinceLastUpdate >= cycleDuration) && (new_passengers_.size() >= MAX_OBJECTS)) {
            // Note queue is full
            std::unique_lock<std::mutex> lck(mtx_);
            std::cout << "Queue full, no new passenger generated." << std::endl;
            lck.unlock();
            // Reset stop watch so wait a bit to see if queue frees up
            lastUpdate = std::chrono::system_clock::now();
        }

        // Read and act on any messages
        ReadMessages();

        // Request rides for passengers in queue, if not yet requested
        for (auto passenger_pair : new_passengers_) {
            if (!(passenger_pair.second->RideRequested())) {
                RequestRide(passenger_pair.second);
            }
        }
    }
}

void PassengerQueue::Message(SimpleMessage simple_message) {
    std::lock_guard<std::mutex> lck(messages_mutex_);
    // Add the message for later reading
    messages_.emplace_back(simple_message);
}

void PassengerQueue::ReadMessages() {
    // Lock and copy over the messages so can release the mutex faster
    std::unique_lock<std::mutex> lck(messages_mutex_);
    std::vector<SimpleMessage> copied_messages(messages_);
    // Clear out the original messages and unlock
    messages_.clear();
    lck.unlock();

    // Take action based on each message code
    for (auto message : copied_messages) {
        if (message.message_code == MsgCodes::ride_on_way) {
            RideOnWay(message.id);
        } else if (message.message_code == MsgCodes::ride_arrived) {
            RideArrived(message.id);
        } else if (message.message_code == MsgCodes::passenger_picked_up) {
            PassengerPickedUp(message.id);
        } else if (message.message_code == MsgCodes::passenger_failure) {
            PassengerFailure(message.id);
        }
    }
}

void PassengerQueue::RequestRide(std::shared_ptr<Passenger> passenger) {
    passenger->SetRideRequest(true);
    if (ride_matcher_ != nullptr) {
        ride_matcher_->Message({ .message_code=RideMatcher::passenger_requests_ride, .id=passenger->Id() });
    }
}

void PassengerQueue::RideOnWay(int id) {
    // Nothing to do here...yet
}

void PassengerQueue::RideArrived(int id) {
    // Send the passenger to the vehicle
    ride_matcher_->Message({ .message_code=RideMatcher::passenger_to_vehicle, .id=id });
}

void PassengerQueue::PassengerPickedUp(int id) {
    // Erase the passenger from the queue
    new_passengers_.erase(id);
}

void PassengerQueue::PassengerFailure(int id) {
    // Check if enough failures to delete
    auto passenger = new_passengers_.at(id);
    bool remove = passenger->MovementFailure();
    if (remove) {
        // Notify the ride matcher
        ride_matcher_->Message({ .message_code=RideMatcher::passenger_is_ineligible, .id=id });
        // Erase the passenger
        new_passengers_.erase(id);
        // Note to console
        std::lock_guard<std::mutex> lck(mtx_);
        std::cout << "Passenger #" << passenger->Id() <<" unreachable multiple times, leaving map." << std::endl;
    } else {
        // Make a new request by setting ride requested to false
        passenger->SetRideRequest(false);
    }
}

}  // namespace rideshare
