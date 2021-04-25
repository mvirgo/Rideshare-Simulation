#include "PassengerQueue.h"

PassengerQueue::PassengerQueue() {};

PassengerQueue::PassengerQueue(RouteModel *model) {
    model_ = model;
    // TODO: Add simulation instead of creating passengers at start
    for (int i = 0; i < MAX_PASSENGERS; ++i) {
        GeneratePassenger();
    }
}

void PassengerQueue::GeneratePassenger() {
    // TODO: Add appropriate handling of Passenger to avoid memory leaks once made a pointer
    // TODO: Maybe add an id for passenger so can grab desired one?
    auto start = model_->GetRandomMapPosition();
    auto dest = model_->GetRandomMapPosition();
    Passenger passenger = Passenger(start[0], start[1], dest[0], dest[1]);
    new_passengers_.emplace_back(passenger);
}

void PassengerQueue::Simulate() {
    // TODO: Implement
}