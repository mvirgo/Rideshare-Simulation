#ifndef CONCURRENTOBJECT_H
#define CONCURRENTOBJECT_H

#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include "RouteModel.h"

class ConcurrentObject {
  public:
    // constructor / destructor
    ConcurrentObject() {};
    ConcurrentObject(RouteModel *model) : model_(model) {};
    ~ConcurrentObject();

    virtual void Simulate() {};

  protected:
    virtual void GenerateNew() {};
    int MAX_OBJECTS = 10; // TODO: Change or remove limit?
    RouteModel *model_;
    std::vector<std::thread> threads; // holds all threads that have been launched within this object
    static std::mutex _mtx;           // mutex shared by all traffic objects for protecting cout
  
  private:
    static int _idCnt; // global variable for counting object ids
};

#endif