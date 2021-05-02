#include "Vehicle.h"
#include "Passenger.h"

void Vehicle::ResetPathAndIndex() {
    path_.clear();
    path_index_ = 0;
}

void Vehicle::SetPosition(double x, double y) {
    x_ = x;
    y_ = y;
    // If there is a passenger, match to the vehicle's position
    if (passenger_ != nullptr) {
        passenger_->SetPosition(x, y);
    }
}