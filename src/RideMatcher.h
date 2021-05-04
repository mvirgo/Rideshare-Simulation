#ifndef RIDE_MATCHER_H
#define RIDE_MATCHER_H

#include <unordered_map>
#include <set>
#include <thread>
#include <vector>

// Avoid include cycle
class Passenger;
class PassengerQueue;
class VehicleManager;

class RideMatcher : std::enable_shared_from_this<RideMatcher> {
  public:
    // Constructor / Destructor
    RideMatcher(std::shared_ptr<PassengerQueue> passenger_queue, std::shared_ptr<VehicleManager> vehicle_manager_) :
      passenger_queue_(passenger_queue), vehicle_manager_(vehicle_manager_) {};
    ~RideMatcher();

    // Concurrent simulation
    void Simulate();

    // Pre-Matching
    void PassengerRequestsRide(std::shared_ptr<Passenger> passenger);
    void VehicleRequestsPassenger(int v_id);

    // Post-matching
    void VehicleHasArrived(int v_id);
    void PassengerToVehicle(std::shared_ptr<Passenger> passenger);

  private:
    std::shared_ptr<PassengerQueue> passenger_queue_;
    std::shared_ptr<VehicleManager> vehicle_manager_;
    std::unordered_map<int, std::shared_ptr<Passenger>> passenger_ids_;
    std::set<int> vehicle_ids_;
    std::unordered_map<int, int> vehicle_to_passenger_match_;
    std::unordered_map<int, int> passenger_to_vehicle_match_;
    std::vector<std::thread> threads_;
    void MatchRides();
};

#endif