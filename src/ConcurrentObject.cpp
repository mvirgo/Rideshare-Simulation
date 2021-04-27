#include "ConcurrentObject.h"
#include <algorithm>

// init static variable
int ConcurrentObject::_idCnt = 0;

std::mutex ConcurrentObject::_mtx;

ConcurrentObject::~ConcurrentObject() {
    // set up thread barrier before this object is destroyed
    // std::for_each(threads.begin(), threads.end(), [](std::thread &t) {
    //     t.join();
    // });
}