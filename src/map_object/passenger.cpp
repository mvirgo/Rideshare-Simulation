/**
 * @file passenger.cpp
 * @brief Implementation of a passenger, particularly walking movement.
 *
 * @copyright Copyright (c) 2021, Michael Virgo, released under the MIT License.
 *
 */

#include "passenger.h"

namespace rideshare {

void Passenger::IncrementalMove() {
  // Check distance to next position vs. distance can go b/w timesteps
  double distance = std::sqrt(std::pow(walk_to_pos_.x - position_.x, 2) + std::pow(walk_to_pos_.y - position_.y, 2));

  if (distance <= distance_per_cycle_) {
      // Don't need to calculate intermediate point, just set position as next_pos
      SetPosition((Coordinate){.x = walk_to_pos_.x, .y = walk_to_pos_.y});
      // Set status as at ride
      SetStatus(Passenger::PassengerStatus::at_ride);
  } else {
      // Calculate an intermediate position
      SetPosition(GetIntermediatePosition(walk_to_pos_.x, walk_to_pos_.y));
  }
}

}
