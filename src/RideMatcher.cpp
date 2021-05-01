#include "RideMatcher.h"
#include <algorithm>

#include "Passenger.h"
#include "PassengerQueue.h"
#include "VehicleManager.h"

RideMatcher::~RideMatcher() {
    // Set up thread barrier before this object is destroyed
    std::for_each(threads_.begin(), threads_.end(), [](std::thread &t) {
        t.join();
    });
}

void RideMatcher::PassengerRequestsRide(int id) {
    passenger_ids_.emplace(id);
}

void RideMatcher::VehicleRequestsPassenger(int id) {
    vehicle_ids_.emplace(id);
}

void RideMatcher::VehicleHasArrived(int id) {
    // TODO: Tell PassengerQueue to send passenger to vehicle
}

void RideMatcher::PassengerToVehicle(std::shared_ptr<Passenger> passenger) {
    // TODO: Add passenger to related vehicle
    int v_id = passenger_to_vehicle_match_.at(passenger->Id());
}

void RideMatcher::Simulate() {
    // Launch MatchRides function in a thread
    threads_.emplace_back(std::thread(&RideMatcher::MatchRides, this));
}

void RideMatcher::MatchRides() {
    while (true) {
        // Sleep at every iteration to reduce CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        // Match rides if more than one in each related queue
        if (passenger_ids_.size() > 0 && vehicle_ids_.size() > 0) {
            // TODO: Improve ride matching beyond just grabbing "first" of each
            // Match rides
            int p_id = *passenger_ids_.begin();
            int v_id = *vehicle_ids_.begin();
            vehicle_to_passenger_match_.insert({v_id, p_id});
            passenger_to_vehicle_match_.insert({p_id, v_id});
            // TODO: Notify PassengerQueue and VehicleManager
            // TODO: Remove the ids from the vectors
            passenger_ids_.erase(p_id);
            vehicle_ids_.erase(v_id);
            // TODO: Output the match to console?
        }
    }
}