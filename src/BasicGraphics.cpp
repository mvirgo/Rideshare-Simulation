#include <chrono>
#include <iostream>
#include <thread>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include "BasicGraphics.h"

BasicGraphics::BasicGraphics(float min_lat, float min_lon, float max_lat, float max_lon) {
    min_lat_ = min_lat;
    min_lon_ = min_lon;
    max_lat_ = max_lat;
    max_lon_ = max_lon;
}

void BasicGraphics::Simulate() {
    this->LoadBackgroundImg();
    while (true) {
        // sleep at every iteration to reduce CPU usage
        // TODO: Shorten below delay once ready for active updating
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));

        // update graphics
        this->DrawSimulation();
    }
}

void BasicGraphics::LoadBackgroundImg() {
    // create window
    windowName_ = "Rideshare Simulation";
    cv::namedWindow(windowName_, cv::WINDOW_NORMAL);

    // load image and create copy to be used for semi-transparent overlay
    cv::Mat background = cv::imread(bgFilename_);
    images_.push_back(background);         // first element is the original background
    images_.push_back(background.clone()); // second element will be the transparent overlay
    images_.push_back(background.clone()); // third element will be the result image for display
}

void BasicGraphics::DrawSimulation() {
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

void BasicGraphics::DrawPassengers(float img_rows, float img_cols) {
    // create overlay from passengers
    for (auto const& passenger : passenger_queue_->NewPassengers()) {
        DrawPassenger(img_rows, img_cols, 25, passenger); // Full size marker
    }

    float opacity = 0.85;
    cv::addWeighted(images_.at(1), opacity, images_.at(0), 1.0 - opacity, 0, images_.at(2));
}

void BasicGraphics::DrawPassenger(float img_rows, float img_cols, int marker_size, const std::unique_ptr<Passenger> &passenger) {
        std::vector<float> curr_position = passenger->GetPosition();
        std::vector<float> dest_position = passenger->GetDestination();

        // Adjust the position based on lat & lon in image
        curr_position[0] = (curr_position[0] - min_lon_) / (max_lon_ - min_lon_);
        curr_position[1] = (max_lat_ - curr_position[1]) / (max_lat_ - min_lat_);
        dest_position[0] = (dest_position[0] - min_lon_) / (max_lon_ - min_lon_);
        dest_position[1] = (max_lat_ - dest_position[1]) / (max_lat_ - min_lat_);

        // Draw both current position and destination
        cv::Scalar color = cv::Scalar(passenger->Blue(), passenger->Green(), passenger->Red());
        cv::drawMarker(images_.at(1), cv::Point2d((int)(curr_position[0] * img_cols), (int)(curr_position[1] * img_rows)), color, passenger->PassShape(), 25, 15);
        cv::drawMarker(images_.at(1), cv::Point2d((int)(dest_position[0] * img_cols), (int)(dest_position[1] * img_rows)), color, passenger->DestShape(), 25, 5);
}

void BasicGraphics::DrawVehicles(float img_rows, float img_cols) {
    // create overlay from vehicles
    for (auto vehicle : vehicle_manager_->Vehicles()) {
        std::vector<float> position = vehicle.GetPosition();

        // Adjust the position based on lat & lon in image
        position[0] = (position[0] - min_lon_) / (max_lon_ - min_lon_);
        position[1] = (max_lat_ - position[1]) / (max_lat_ - min_lat_);

        // Set color according to vehicle and draw a marker there
        cv::Scalar color = cv::Scalar(vehicle.Blue(), vehicle.Green(), vehicle.Red());
        cv::drawMarker(images_.at(1), cv::Point2d((int)(position[0] * img_cols), (int)(position[1] * img_rows)), color, vehicle.Shape(), 25, 15);
        // TODO: Also draw any related information for given passenger?
    }

    float opacity = 0.85;
    cv::addWeighted(images_.at(1), opacity, images_.at(0), 1.0 - opacity, 0, images_.at(2));
}