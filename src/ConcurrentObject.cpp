#include "ConcurrentObject.h"
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
