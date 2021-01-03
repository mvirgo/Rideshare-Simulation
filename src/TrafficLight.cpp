#include <iostream>
#include <random>
#include <thread>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */
 
template <typename T>
T MessageQueue<T>::receive()
{
    // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait() 
    // to wait for and receive new messages and pull them from the queue using move semantics. 
    // The received object should then be returned by the receive function.
    std::unique_lock<std::mutex> lck(_mutex); // lock down the queue
    _condition.wait(lck, [this] { return !_queue.empty(); }); // wait til queue is not empty
    T msg = std::move(_queue.front()); // Take from front of queue (FIFO)
    _queue.pop_front();

    return msg;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // FP.4a : The method send should use the mechanisms std::lock_guard<std::mutex> 
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.
    std::lock_guard<std::mutex> lck(_mutex); // lock down the queue
    _queue.clear(); // fix bug noted in Knowledge post 98313 that messages back up
    _queue.emplace_back(std::move(msg)); // add to the queue
    _condition.notify_one(); // notify that queue was added to
}


/* Implementation of class "TrafficLight" */
 
TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop 
    // runs and repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.
    while (true) {
        if (_queue.receive() == green) { // return when green light
            return;
        }
    }
}

enum TrafficLight::TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    // FP.2b : Finally, the private method „cycleThroughPhases“ should be started in a thread when the public method „simulate“ is called. To do this, use the thread queue in the base class.
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this)); 
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // FP.2a : Implement the function with an infinite loop that measures the time between two loop cycles 
    // and toggles the current phase of the traffic light between red and green and sends an update method 
    // to the message queue using move semantics. The cycle duration should be a random value between 4 and 6 seconds. 
    // Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles.

    // Set initial start time and random time between cycles (4-6 seconds, aka 4K-6K milliseconds)
    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point current;
    srand(time(0)); // Make sure we get different random values each time for below
    int waitTime = getWaitTime();
    // Infinitely loop
    while (true) {
        // Calculate time span since previous stary
        current = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(current - start).count();
        if (duration > waitTime) {
            // Toggle the light
            if (_currentPhase == red) {
                _currentPhase = green;
            } else {
                _currentPhase = red;
            }
            // Send update to message queue
            _queue.send(std::move(_currentPhase));
            // Set new start and waitTime
            start = current;
            waitTime = getWaitTime();
        }
        // Wait between cycles
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    } 
}

int TrafficLight::getWaitTime() {
    // Return a time between light changes of 4-6 seconds
    // 0-1 float, * 2 (0-2), + 4 (4-6), * 1K for milliseconds
    return ((((float) rand() / RAND_MAX) * 2) + 4) * 1000;
}