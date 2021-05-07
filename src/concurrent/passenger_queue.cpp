#include "passenger_queue.h"

#include <chrono>
#include <mutex>

#include "ride_matcher.h"
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

        // Request rides for passengers in queue, if not yet requested
        for (auto passenger_pair : new_passengers_) {
            if (!(passenger_pair.second->RideRequested())) {
                RequestRide(passenger_pair.second);
            }
        }
    }
}

void PassengerQueue::RequestRide(std::shared_ptr<Passenger> passenger) {
    passenger->SetRideRequest(true);
    if (ride_matcher_ != nullptr) {
        ride_matcher_->PassengerRequestsRide(passenger->Id());
    }
}

void PassengerQueue::RideOnWay(int id) {
    // Nothing to do here...yet
}

void PassengerQueue::RideArrived(int id) {
    // Send the passenger to the vehicle
    ride_matcher_->PassengerToVehicle(id);
    // Erase the passenger from the queue
    new_passengers_.erase(id);
}

void PassengerQueue::PassengerFailure(int id) {
    // Check if enough failures to delete
    auto passenger = new_passengers_.at(id);
    bool remove = passenger->MovementFailure();
    if (remove) {
        // Notify the ride matcher
        ride_matcher_->PassengerIsIneligible(id);
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
