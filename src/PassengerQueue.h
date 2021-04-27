#ifndef PASSENGERQUEUE_H
#define PASSENGERQUEUE_H

#include <vector>
#include "ConcurrentObject.h"
#include "Passenger.h"

// TODO: Change this to run concurrently so can simulate adding more
class PassengerQueue : public ConcurrentObject, std::enable_shared_from_this<PassengerQueue> {
  public:
    // constructor / destructor
    PassengerQueue() {};
    PassengerQueue(RouteModel *model);
    
    // getter
    std::vector<Passenger> NewPassengers() { return new_passengers_; }

    void Simulate();

    // miscellaneous
    std::shared_ptr<PassengerQueue> GetSharedThis() { return shared_from_this(); }

  private:
    void GenerateNew();
    std::vector<Passenger> new_passengers_;
};

#endif