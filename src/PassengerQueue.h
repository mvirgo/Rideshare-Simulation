#ifndef PASSENGERQUEUE_H
#define PASSENGERQUEUE_H

#include <vector>
#include "ConcurrentObject.h"
#include "Passenger.h"

// TODO: Change this to run concurrently so can simulate adding more
class PassengerQueue : public ConcurrentObject {
  public:
    // constructor / destructor
    PassengerQueue() {};
    PassengerQueue(RouteModel *model);
    
    // getter
    std::vector<Passenger> NewPassengers() { return new_passengers_; }

    void Simulate();

  private:
    void GenerateNew();
    std::vector<Passenger> new_passengers_;
};

#endif