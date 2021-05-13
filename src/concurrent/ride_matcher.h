/**
 * @file ride_matcher.h
 * @brief Handles matching of passengers to vehicles.
 *
 * @copyright Copyright (c) 2021, Michael Virgo, released under the MIT License.
 *
 */

#ifndef RIDE_MATCHER_H_
#define RIDE_MATCHER_H_

#include <unordered_map>
#include <set>
#include <thread>
#include <utility>

#include "concurrent_object.h"
#include "message_handler.h"
#include "passenger_queue.h"
#include "simple_message.h"
#include "vehicle_manager.h"
#include "map_object/passenger.h"

namespace rideshare {

class RideMatcher : public ConcurrentObject, public MessageHandler {
  public:
    // Messages are received with below enum code and either vehicle or passenger id
    enum MsgCodes {
        passenger_requests_ride,
        vehicle_requests_passenger,
        vehicle_cannot_reach_passenger,
        vehicle_has_arrived,
        passenger_to_vehicle,
        passenger_is_ineligible,
        vehicle_is_ineligible,
    };

    // Constructor / Destructor
    RideMatcher(std::shared_ptr<PassengerQueue> passenger_queue, std::shared_ptr<VehicleManager> vehicle_manager_, double map_dim) :
      passenger_queue_(passenger_queue), vehicle_manager_(vehicle_manager_),
      CLOSE_ENOUGH_(map_dim * MAP_FRACTION) {};

    // Concurrent simulation
    void Simulate();

    // Message receiving
    void Message(SimpleMessage simple_message);

  private:
    // Pre-Matching
    void PassengerRequestsRide(int p_id);
    void VehicleRequestsPassenger(int v_id);

    // Matching
    void MatchRides();
    void ClosestMatch();
    void SimpleMatch();
    bool MatchIsValid(int p_id, int v_id);
    void ProcessSingleMatch(int p_id, int v_id);
    void NoPossibleMatch(int p_id);

    // Post-Matching
    void VehicleCannotReachPassenger(int v_id);
    void VehicleHasArrived(int v_id);
    void PassengerToVehicle(int p_id);

    // Removal
    void PassengerIsIneligible(int p_id);
    void VehicleIsIneligible(int v_id);

    // Message reading
    void ReadMessages();

    // Utility
    void ClearInvalids(int p_id);
    double Distance(Coordinate p_loc, Coordinate v_loc);

    // Member variables
    std::shared_ptr<PassengerQueue> passenger_queue_;
    std::shared_ptr<VehicleManager> vehicle_manager_;
    std::set<int> passenger_ids_;
    std::set<int> vehicle_ids_;
    std::unordered_map<int, int> vehicle_to_passenger_match_;
    std::unordered_map<int, int> passenger_to_vehicle_match_;
    std::set<std::pair<int, int>> invalid_matches_; // p_id, v_id
    const double MAP_FRACTION = 0.15; // Fraction of map to be "close enough"
    const double CLOSE_ENOUGH_;
};

}  // namespace rideshare

#endif  // RIDE_MATCHER_H_
