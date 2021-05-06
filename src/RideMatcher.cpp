#include "RideMatcher.h"
#include <algorithm>

#include "Passenger.h"
#include "PassengerQueue.h"
#include "VehicleManager.h"

void RideMatcher::PassengerRequestsRide(std::shared_ptr<Passenger> passenger) {
    passenger_ids_.emplace(passenger->Id(), passenger);
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
    // Notify passenger of failure
    passenger_queue_->PassengerFailure(p_id);
    // Output the un-match to console
    std::lock_guard<std::mutex> lck(mtx_);
    std::cout << "Vehicle #" << v_id << " un-matched from Passenger #" << p_id << ", unreachable." << std::endl;
}

void RideMatcher::VehicleHasArrived(int v_id) {
    // Tell PassengerQueue to send passenger to vehicle
    int p_id = vehicle_to_passenger_match_.at(v_id);
    passenger_queue_->RideArrived(p_id);
}

void RideMatcher::PassengerToVehicle(std::shared_ptr<Passenger> passenger) {
    // Add passenger to related vehicle
    int v_id = passenger_to_vehicle_match_.at(passenger->Id());
    vehicle_manager_->PassengerIntoVehicle(v_id, passenger);
    // Remove both from match maps
    passenger_to_vehicle_match_.erase(passenger->Id());
    vehicle_to_passenger_match_.erase(v_id);
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
        // TODO: Notify vehicle of failure
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
        passenger_queue_->PassengerFailure(p_id);
    }
}

void RideMatcher::Simulate() {
    // Launch MatchRides function in a thread
    threads_.emplace_back(std::thread(&RideMatcher::MatchRides, this));
}

void RideMatcher::MatchRides() {
    while (true) {
        // Sleep at every iteration to reduce CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        // Match rides if more than one in each related queue
        if (passenger_ids_.size() > 0 && vehicle_ids_.size() > 0) {
            // TODO: Improve ride matching beyond just grabbing "first" of each
            // Match rides
            auto passenger_info = *passenger_ids_.begin();
            int p_id = passenger_info.first;
            int v_id = *vehicle_ids_.begin();
            vehicle_to_passenger_match_.insert({v_id, p_id});
            passenger_to_vehicle_match_.insert({p_id, v_id});
            // Notify PassengerQueue and VehicleManager
            vehicle_manager_->AssignPassenger(v_id, passenger_info.second->GetPosition());
            passenger_queue_->RideOnWay(p_id);
            // Remove the ids from the vectors
            passenger_ids_.erase(p_id);
            vehicle_ids_.erase(v_id);
            // Output the match to console
            std::lock_guard<std::mutex> lck(mtx_);
            std::cout << "Vehicle #" << v_id << " matched to Passenger #" << p_id << "." << std::endl;
        }
    }
}