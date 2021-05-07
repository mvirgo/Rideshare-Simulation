/**
 * @file concurrent_object.h
 * @brief Parent for concurrent simulation classes using threads.
 *
 * @copyright Copyright (c) 2021, Michael Virgo, released under the MIT License.
 *
 */

#ifndef CONCURRENT_OBJECT_H_
#define CONCURRENT_OBJECT_H_

#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

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

#endif  // CONCURRENT_OBJECT_H_
