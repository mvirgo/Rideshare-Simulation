#ifndef PASSENGERQUEUE_H
#define PASSENGERQUEUE_H

#include <vector>
#include "Passenger.h"
#include "RouteModel.h"

// TODO: Change this to run concurrently so can simulate adding more
class PassengerQueue {
  public:
    // constructor / destructor
    PassengerQueue();
    PassengerQueue(RouteModel *model);
    
    // getter
    std::vector<Passenger> NewPassengers() { return new_passengers_; }

  private:
    void GeneratePassenger();
    void Simulate();
    int MAX_PASSENGERS = 10; // TODO: Change or remove limit?
    RouteModel *model_;
    std::vector<Passenger> new_passengers_;
};

#endif