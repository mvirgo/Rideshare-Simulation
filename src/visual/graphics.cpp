/**
 * @file graphics.cpp
 * @brief Implementation of drawing vehicles and passengers on a map image.
 *
 * @cite Adapted from https://github.com/udacity/CppND-Program-a-Concurrent-Traffic-Simulation
 * @copyright Copyright (c) 2021, Michael Virgo, released under the MIT License.
 *
 */

#include "graphics.h"

#include <chrono>
#include <memory>
#include <thread>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include "mapping/coordinate.h"

namespace rideshare {

Graphics::Graphics(float min_lat, float min_lon, float max_lat, float max_lon) {
    min_lat_ = min_lat;
    min_lon_ = min_lon;
    max_lat_ = max_lat;
    max_lon_ = max_lon;
}

void Graphics::Simulate() {
    this->LoadBackgroundImg();
    while (true) {
        // sleep at every iteration to reduce CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        // update graphics
        this->DrawSimulation();
    }
}

void Graphics::LoadBackgroundImg() {
    // create window
    windowName_ = "Rideshare Simulation";
    cv::namedWindow(windowName_, cv::WINDOW_NORMAL);

    // load image and create copy to be used for semi-transparent overlay
    cv::Mat background = cv::imread(bgFilename_);
    images_.push_back(background);         // first element is the original background
    images_.push_back(background.clone()); // second element will be the transparent overlay
    images_.push_back(background.clone()); // third element will be the result image for display
}

void Graphics::DrawSimulation() {
    // reset images
    images_.at(1) = images_.at(0).clone();
    images_.at(2) = images_.at(0).clone();

    // Get image rows and columns to help with coordinate adjustments for image
    float img_rows = images_.at(0).rows;
    float img_cols = images_.at(0).cols;

    DrawPassengers(img_rows, img_cols);
    DrawVehicles(img_rows, img_cols);

    // display background and overlay image
    cv::imshow(windowName_, images_.at(2));
    cv::waitKey(33);
}

void Graphics::DrawPassengers(float img_rows, float img_cols) {
    // create overlay from passengers
    for (auto const& passenger_map : passenger_queue_->NewPassengers()) {
        DrawPassenger(img_rows, img_cols, 25, passenger_map.second); // Full size marker when waiting
    }
    for (auto const& walking_passenger : passenger_queue_->WalkingPassengers()) {
        DrawPassenger(img_rows, img_cols, 15, walking_passenger.second); // Smaller marker when walking
    }

    float opacity = 0.85;
    cv::addWeighted(images_.at(1), opacity, images_.at(0), 1.0 - opacity, 0, images_.at(2));
}

void Graphics::DrawPassenger(float img_rows, float img_cols, int marker_size, const std::shared_ptr<Passenger> &passenger) {
        Coordinate curr_position = passenger->GetPosition();
        Coordinate dest_position = passenger->GetDestination();

        // Adjust the position based on lat & lon in image
        curr_position.x = (curr_position.x - min_lon_) / (max_lon_ - min_lon_);
        curr_position.y = (max_lat_ - curr_position.y) / (max_lat_ - min_lat_);
        dest_position.x = (dest_position.x - min_lon_) / (max_lon_ - min_lon_);
        dest_position.y = (max_lat_ - dest_position.y) / (max_lat_ - min_lat_);

        // Draw both current position (size based on if in vehicle or not) and destination (always full-size)
        cv::Scalar color = cv::Scalar(passenger->Blue(), passenger->Green(), passenger->Red());
        cv::drawMarker(images_.at(1), cv::Point2d((int)(curr_position.x * img_cols), (int)(curr_position.y * img_rows)), color, passenger->PassShape(), marker_size, 15);
        cv::drawMarker(images_.at(1), cv::Point2d((int)(dest_position.x * img_cols), (int)(dest_position.y * img_rows)), color, passenger->DestShape(), 25, 5);
}

void Graphics::DrawVehicles(float img_rows, float img_cols) {
    // create overlay from vehicles
    for (auto const & [id, vehicle] : vehicle_manager_->Vehicles()) {
        Coordinate position = vehicle->GetPosition();

        // Adjust the position based on lat & lon in image
        position.x = (position.x - min_lon_) / (max_lon_ - min_lon_);
        position.y = (max_lat_ - position.y) / (max_lat_ - min_lat_);

        // Set color according to vehicle and draw a marker there
        cv::Scalar color = cv::Scalar(vehicle->Blue(), vehicle->Green(), vehicle->Red());
        cv::drawMarker(images_.at(1), cv::Point2d((int)(position.x * img_cols), (int)(position.y * img_rows)), color, vehicle->Shape(), 25, 15);
        // Draw any related information for possible passenger
        auto passenger = vehicle->GetPassenger(); // ensures shared pointer will stay alive while drawing, if it exits
        if (passenger != nullptr) {
            // Note that this state is guaranteed to have a passenger
            DrawPassenger(img_rows, img_cols, 15, passenger); // Smaller marker
        }
    }

    float opacity = 0.85;
    cv::addWeighted(images_.at(1), opacity, images_.at(0), 1.0 - opacity, 0, images_.at(2));
}

}  // namespace rideshare
