#include "PassengerQueue.h"
#include <chrono>

PassengerQueue::PassengerQueue(RouteModel *model) : ConcurrentObject(model) {
    // Start by creating half the max number of passengers
    for (int i = 0; i < MAX_OBJECTS / 2; ++i) {
        GenerateNew();
    }
}

void PassengerQueue::GenerateNew() {
    // TODO: Add appropriate handling of Passenger to avoid memory leaks once made a pointer
    // TODO: Maybe add an id for passenger so can grab desired one?
    auto start = model_->GetRandomMapPosition();
    auto dest = model_->GetRandomMapPosition();
    Passenger passenger = Passenger();
    passenger.SetPosition(start[0], start[1]);
    passenger.SetDestination(dest[0], dest[1]);
    new_passengers_.emplace_back(passenger);
    // TODO: Output id of passenger requesting ride
}

void PassengerQueue::Simulate() {
    // Launch WaitForRide function in a thread
    threads.emplace_back(std::thread(&PassengerQueue::WaitForRide, this));
}

void PassengerQueue::WaitForRide() {
    // Set wait time between potentially generating new passengers
    double cycleDuration = ((((float) rand() / RAND_MAX) * RANGE_WAIT_TIME_) + MIN_WAIT_TIME_) * 1000; // duration of a single simulation cycle in ms
    std::chrono::time_point<std::chrono::system_clock> lastUpdate = std::chrono::system_clock::now();

    while (true) {
        // Sleep at every iteration to reduce CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        // Compute time difference to stop watch
        long timeSinceLastUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - lastUpdate).count();

        // Check if cycleDuration passed and if less than max passengers before creating a new one
        if ((timeSinceLastUpdate >= cycleDuration) && (new_passengers_.size() < MAX_OBJECTS)) {
            GenerateNew();
            // Get a new random time to wait before checking to add a new passenger
            cycleDuration = ((((float) rand() / RAND_MAX) * RANGE_WAIT_TIME_) + MIN_WAIT_TIME_) * 1000;
            // Reset stop watch
            lastUpdate = std::chrono::system_clock::now();
        }
    }
}