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
    const std::vector<std::unique_ptr<Passenger>>& NewPassengers() { return new_passengers_; }

    void Simulate();

    // miscellaneous
    std::shared_ptr<PassengerQueue> GetSharedThis() { return shared_from_this(); }

  private:
    void WaitForRide();
    void GenerateNew();
    int MIN_WAIT_TIME_ = 3; // seconds to wait between generation attempts
    int RANGE_WAIT_TIME_ = 2; // range in seconds to wait between generation attempts
    std::vector<std::unique_ptr<Passenger>> new_passengers_;
};

#endif