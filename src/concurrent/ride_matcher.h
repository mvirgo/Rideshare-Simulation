/**
 * @file ride_matcher.h
 * @brief Handles matching of passengers to vehicles.
 *
 * @copyright Copyright (c) 2021, Michael Virgo, released under the MIT License.
 *
 */

#ifndef RIDE_MATCHER_H_
#define RIDE_MATCHER_H_

#include <memory>
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
    RideMatcher(std::shared_ptr<PassengerQueue> passenger_queue,
                std::shared_ptr<VehicleManager> vehicle_manager_,
                double map_dim, std::string match_type) :
      passenger_queue_(passenger_queue), vehicle_manager_(vehicle_manager_),
      CLOSE_ENOUGH_(map_dim * MAP_FRACTION_), MATCH_TYPE_(match_type) {};

    // Concurrent simulation
    void Simulate();

    // Message receiving
    void Message(SimpleMessage simple_message);

  private:
    // Pre-Matching
    // A given passenger requests to be matched with a ride
    void PassengerRequestsRide(int p_id);
    // A given vehicle requests to be matched to a rider
    void VehicleRequestsPassenger(int v_id);

    // Matching
    // Handles loop cycle of a single match at a time
    void MatchRides();
    // Matches earliest passenger ID (close to FIFO) to a close or closest vehicle
    void ClosestMatch();
    // Matches earliest passenger ID to earliest available vehicle ID
    void SimpleMatch();
    // Checks whether a given match was previously invalid due to being unreachable
    bool MatchIsValid(int p_id, int v_id);
    // Once match is determined, removes both sides from queue and notifies the related parties
    void ProcessSingleMatch(int p_id, int v_id);
    // No match is possible for the given passenger at this time, so notify them of a failure
    void NoPossibleMatch(int p_id);

    // Post-Matching
    // A given vehicle cannot reach their matched passenger, so needs to be unmatched
    void VehicleCannotReachPassenger(int v_id);
    // The matched vehicle has arrived at the closest rode node to the matched passenger position
    void VehicleHasArrived(int v_id);
    // Move the passenger into the arrived vehicle, and notify the passenger queue so it can remove
    void PassengerToVehicle(int p_id);

    // Removal
    // A given passenger is being deleted by the passenger queue, and should be un-matched or removed
    void PassengerIsIneligible(int p_id);
    // A given vehicle is being deleted by the vehicle manager, and should be un-matched or removed
    void VehicleIsIneligible(int v_id);

    // Message reading - take action based on given message
    void ReadMessages();

    // Utility
    // Clear out any previous invalid matches stored, as passenger either picked up or ineligible
    void ClearInvalids(int p_id);
    // Calculate Euclidean distance between two coordinates (passenger and vehicle)
    double Distance(Coordinate p_loc, Coordinate v_loc);

    // Member variables
    std::shared_ptr<PassengerQueue> passenger_queue_;
    std::shared_ptr<VehicleManager> vehicle_manager_;
    std::set<int> passenger_ids_;
    std::set<int> vehicle_ids_;
    std::unordered_map<int, int> vehicle_to_passenger_match_;
    std::unordered_map<int, int> passenger_to_vehicle_match_;
    std::set<std::pair<int, int>> invalid_matches_; // p_id, v_id
    const double MAP_FRACTION_ = 0.15; // Fraction of map to be "close enough"
    const double CLOSE_ENOUGH_; // Avg. map dimension * MAP_FRACTION_
    const std::string MATCH_TYPE_; // "closest" or "simple" matching
};

}  // namespace rideshare

#endif  // RIDE_MATCHER_H_
