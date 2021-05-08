/**
 * @file ride_matcher.cpp
 * @brief Implementation of matching (or un-matching) of passengers to vehicles.
 *
 * @copyright Copyright (c) 2021, Michael Virgo, released under the MIT License.
 *
 */

#include "ride_matcher.h"

#include <algorithm>

#include "passenger_queue.h"
#include "simple_message.h"
#include "vehicle_manager.h"
#include "map_object/passenger.h"

namespace rideshare {

void RideMatcher::PassengerRequestsRide(int p_id) {
    passenger_ids_.emplace(p_id);
}

void RideMatcher::VehicleRequestsPassenger(int v_id) {
    vehicle_ids_.emplace(v_id);
}

void RideMatcher::VehicleCannotReachPassenger(int v_id) {
    // Remove the match
    int p_id = vehicle_to_passenger_match_.at(v_id);
    vehicle_to_passenger_match_.erase(v_id);
    passenger_to_vehicle_match_.erase(p_id);
    // TODO: Somehow track to make sure don't re-assign this pair
    // Output the un-match to console
    std::unique_lock<std::mutex> lck(mtx_);
    std::cout << "Vehicle #" << v_id << " un-matched from Passenger #" << p_id << ", unreachable." << std::endl;
    lck.unlock();
    // Notify passenger of failure
    passenger_queue_->Message({ .message_code = PassengerQueue::MsgCodes::passenger_failure, .id = p_id });
}

void RideMatcher::VehicleHasArrived(int v_id) {
    // Tell PassengerQueue to send passenger to vehicle
    int p_id = vehicle_to_passenger_match_.at(v_id);
    passenger_queue_->Message({ .message_code = PassengerQueue::MsgCodes::ride_arrived, .id = p_id });
}

void RideMatcher::PassengerToVehicle(int p_id) {
    auto passenger = passenger_queue_->NewPassengers().at(p_id);
    // Add passenger to related vehicle
    int v_id = passenger_to_vehicle_match_.at(p_id);
    vehicle_manager_->PassengerIntoVehicle(v_id, passenger);
    // Remove both from match maps
    passenger_to_vehicle_match_.erase(p_id);
    vehicle_to_passenger_match_.erase(v_id);
    // Let passenger queue know passenger was picked up
    passenger_queue_->Message({ .message_code = PassengerQueue::MsgCodes::passenger_picked_up, .id = p_id });
}

void RideMatcher::PassengerIsIneligible(int p_id) {
    // Remove passenger
    passenger_ids_.erase(p_id);
    // Check for any associated match
    if (passenger_to_vehicle_match_.count(p_id) == 1) {
        // Found a match, remove both sides
        int v_id = passenger_to_vehicle_match_.at(p_id);
        vehicle_to_passenger_match_.erase(v_id);
        passenger_to_vehicle_match_.erase(p_id);
        // Note: Currently do not need to notify vehicle of failure,
        //  only way to get here is through vehicle issues first
    }
}

void RideMatcher::VehicleIsIneligible(int v_id) {
    // Remove vehicle
    vehicle_ids_.erase(v_id);
    // Check for any associated match
    if (vehicle_to_passenger_match_.count(v_id) == 1) {
        // Found a match, remove both sides
        int p_id = vehicle_to_passenger_match_.at(v_id);
        vehicle_to_passenger_match_.erase(v_id);
        passenger_to_vehicle_match_.erase(p_id);
        // Notify passenger of failure
        passenger_queue_->Message({ .message_code = PassengerQueue::MsgCodes::passenger_failure, .id = p_id });
    }
}

void RideMatcher::Simulate() {
    // Launch MatchRides function in a thread
    threads.emplace_back(std::thread(&RideMatcher::MatchRides, this));
}

void RideMatcher::MatchRides() {
    while (true) {
        // Sleep at every iteration to reduce CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        // Read and act on any messages
        ReadMessages();

        // Match rides if more than one in each related queue
        if (passenger_ids_.size() > 0 && vehicle_ids_.size() > 0) {
            // TODO: Improve ride matching beyond just grabbing "first" of each
            // Match rides
            int p_id = *passenger_ids_.begin();
            int v_id = *vehicle_ids_.begin();
            vehicle_to_passenger_match_.insert({v_id, p_id});
            passenger_to_vehicle_match_.insert({p_id, v_id});
            // Remove the ids from the sets
            passenger_ids_.erase(p_id);
            vehicle_ids_.erase(v_id);
            // Output the match to console
            std::unique_lock<std::mutex> lck(mtx_);
            std::cout << "Vehicle #" << v_id << " matched to Passenger #" << p_id << "." << std::endl;
            lck.unlock();
            // Notify PassengerQueue and VehicleManager
            vehicle_manager_->AssignPassenger(v_id, passenger_queue_->NewPassengers().at(p_id)->GetPosition());
            passenger_queue_->Message({ .message_code = PassengerQueue::MsgCodes::ride_on_way, .id = p_id });
        }
    }
}

void RideMatcher::Message(SimpleMessage simple_message) {
    std::lock_guard<std::mutex> lck(messages_mutex_);
    // Add the message for later reading
    messages_.emplace_back(simple_message);
}

void RideMatcher::ReadMessages() {
    std::lock_guard<std::mutex> lck(messages_mutex_);
    // Take action based on each message code
    for (auto message : messages_) {
        switch (message.message_code) {
            case MsgCodes::passenger_requests_ride:
                PassengerRequestsRide(message.id);
                break;
            case MsgCodes::vehicle_requests_passenger:
                VehicleRequestsPassenger(message.id);
                break;
            case MsgCodes::vehicle_cannot_reach_passenger:
                VehicleCannotReachPassenger(message.id);
                break;
            case MsgCodes::vehicle_has_arrived:
                VehicleHasArrived(message.id);
                break;
            case MsgCodes::passenger_to_vehicle:
                PassengerToVehicle(message.id);
                break;
            case MsgCodes::passenger_is_ineligible:
                PassengerIsIneligible(message.id);
                break;
            case MsgCodes::vehicle_is_ineligible:
                VehicleIsIneligible(message.id);
                break;
            default:
                // Invalid message, ignore
                continue;
        }
    }
    // Clear out the messages
    messages_.clear();
}

}  // namespace rideshare
