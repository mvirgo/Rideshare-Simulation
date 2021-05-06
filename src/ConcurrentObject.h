#ifndef CONCURRENT_OBJECT_H
#define CONCURRENT_OBJECT_H

#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

#include "RouteModel.h"
#include "RoutePlanner.h"

namespace rideshare {

class ConcurrentObject {
  public:
    // Constructor / Destructor
    ~ConcurrentObject();

    virtual void Simulate() {};

  protected:
    std::vector<std::thread> threads; // Holds all threads that have been launched within this object
    static std::mutex mtx_;           // Mutex shared by all concurrent objects for protecting cout
};

}  // namespace rideshare

#endif
