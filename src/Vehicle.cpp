#include "Vehicle.h"
#include "Passenger.h"

void Vehicle::SetPassenger(std::shared_ptr<Passenger> passenger) {
    passenger_ = passenger;
    // Set passenger's destination as the vehicle's destination
    auto new_dest = passenger->GetDestination();
    dest_x_ = new_dest[0];
    dest_y_ = new_dest[1];
}

void Vehicle::SetPosition(double x, double y) {
    x_ = x;
    y_ = y;
    // If there is a passenger, match to the vehicle's position
    if (passenger_ != nullptr) {
        passenger_->SetPosition(x, y);
    }
}

void Vehicle::ResetPathAndIndex() {
    path_.clear();
    path_index_ = 0;
}