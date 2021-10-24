/**
 * @file vehicle.cpp
 * @brief Implementation of a vehicle, particularly handling position, path and passengers.
 *
 * @copyright Copyright (c) 2021, Michael Virgo, released under the MIT License.
 *
 */

#include "vehicle.h"

#include <memory>

#include "mapping/coordinate.h"
#include "mapping/model.h"
#include "passenger.h"

namespace rideshare {

void Vehicle::SetPassenger(std::shared_ptr<Passenger> passenger) {
    passenger_ = passenger;
    // Set passenger's destination as the vehicle's destination
    SetDestination(passenger->GetDestination());
}

void Vehicle::SetPosition(const Coordinate &position) {
    position_ = position;
    // If there is a passenger, match to the vehicle's position
    if (passenger_ != nullptr) {
        passenger_->SetPosition(position);
    }
}

void Vehicle::SetDestination(const Coordinate &destination) {
    destination_ = destination;
    // Reset the path and index so will properly route on a new path
    ResetPathAndIndex();
}

void Vehicle::DropOffPassenger() {
    // Clear out the passenger
    passenger_.reset();
    // Clear out failures as well, since had a successful ride
    failures_ = 0;
    // TODO: May want more post-dropoff later
}

void Vehicle::IncrementalMove() {
    Model::Node next_pos = path_.at(path_index_);
    // Check distance to next position vs. distance can go b/w timesteps
    double distance = std::sqrt(std::pow(next_pos.x - position_.x, 2) + std::pow(next_pos.y - position_.y, 2));

    if (distance <= distance_per_cycle_) {
        // Don't need to calculate intermediate point, just set position as next_pos
        SetPosition((Coordinate){.x = next_pos.x, .y = next_pos.y});
        IncrementPathIndex();
    } else {
        // Calculate an intermediate position
        SetPosition(GetIntermediatePosition(next_pos.x, next_pos.y));
    }
}

void Vehicle::ResetPathAndIndex() {
    path_.clear();
    path_index_ = 0;
}

}  // namespace rideshare
