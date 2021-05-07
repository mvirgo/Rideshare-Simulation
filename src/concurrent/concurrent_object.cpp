/**
 * @file concurrent_object.cpp
 * @brief Initialization of shared concurrent object cout mutex and destructor of threads.
 *
 * @copyright Copyright (c) 2021, Michael Virgo, released under the MIT License.
 *
 */

#include "concurrent_object.h"

#include <algorithm>

namespace rideshare {

// init static variable
std::mutex ConcurrentObject::mtx_;

ConcurrentObject::~ConcurrentObject() {
    // set up thread barrier before this object is destroyed
    std::for_each(threads.begin(), threads.end(), [](std::thread &t) {
        t.join();
    });
}

}  // namespace rideshare
