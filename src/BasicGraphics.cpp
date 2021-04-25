#include <chrono>
#include <iostream>
#include <thread>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include "BasicGraphics.h"

BasicGraphics::BasicGraphics(float minLat, float minLon, float maxLat, float maxLon)
{
    this->minLat = minLat;
    this->minLon = minLon;
    this->maxLat = maxLat;
    this->maxLon = maxLon;
}

void BasicGraphics::simulate()
{
    this->loadBackgroundImg();
    while (true)
    {
        // sleep at every iteration to reduce CPU usage
        // TODO: Shorten below delay once ready for active updating
        std::this_thread::sleep_for(std::chrono::milliseconds(10000));

        // update graphics
        this->drawSimulation();
    }
}

void BasicGraphics::loadBackgroundImg()
{
    // create window
    _windowName = "Rideshare Simulation";
    cv::namedWindow(_windowName, cv::WINDOW_NORMAL);

    // load image and create copy to be used for semi-transparent overlay
    cv::Mat background = cv::imread(_bgFilename);
    _images.push_back(background);         // first element is the original background
    _images.push_back(background.clone()); // second element will be the transparent overlay
    _images.push_back(background.clone()); // third element will be the result image for display
}

void BasicGraphics::drawSimulation()
{
    // reset images
    _images.at(1) = _images.at(0).clone();
    _images.at(2) = _images.at(0).clone();

    drawIntersections();
    drawPassengers();

    // display background and overlay image
    cv::imshow(_windowName, _images.at(2));
    cv::waitKey(33);
}

void BasicGraphics::drawIntersections()
{
    // Get image rows and columns to help with coordinate adjustments for image
    float imgRows = _images.at(0).rows;
    float imgCols = _images.at(0).cols;

    // create overlay from intersections
    for (auto intersect : _intersections)
    {
        std::vector<float> position = intersect.getPosition();

        // Adjust the position based on lat & lon in image
        position[0] = (position[0] - minLon) / (maxLon - minLon);
        position[1] = (maxLat - position[1]) / (maxLat - minLat);

        // set color according to traffic light and draw the intersection as a circle
        //std::cout << "Position at: " << (int)(position[0] * imgCols) << "," << (int)(position[1] * imgRows) << std::endl;
        cv::circle(_images.at(1), cv::Point2d((int)(position[0] * imgCols), (int)(position[1] * imgRows)), 25, cv::Scalar(0, 255, 0), -1);
    }

    float opacity = 0.85;
    cv::addWeighted(_images.at(1), opacity, _images.at(0), 1.0 - opacity, 0, _images.at(2));
    std::cout << "Image size is: " << _images.at(0).rows << " rows, and " << _images.at(0).cols << " columns." << std::endl;
}

void BasicGraphics::drawPassengers() {
        // Get image rows and columns to help with coordinate adjustments for image
    float imgRows = _images.at(0).rows;
    float imgCols = _images.at(0).cols;

    // create overlay from passengers
    for (auto passenger : passenger_queue_.newPassengers())
    {
        std::vector<float> start_position = {passenger.startX(), passenger.startY()};
        std::vector<float> dest_position = {passenger.destX(), passenger.destY()};

        // Adjust the position based on lat & lon in image
        // TODO: Change below to just need one position
        start_position[0] = (start_position[0] - minLon) / (maxLon - minLon);
        start_position[1] = (maxLat - start_position[1]) / (maxLat - minLat);
        dest_position[0] = (dest_position[0] - minLon) / (maxLon - minLon);
        dest_position[1] = (maxLat - dest_position[1]) / (maxLat - minLat);

        // TODO: Change to be just given position of passenger, with different colors for each passenger
        // set blue for start and red for finish
        //std::cout << "Position at: " << (int)(start_position[0] * imgCols) << "," << (int)(start_position[1] * imgRows) << std::endl;
        cv::circle(_images.at(1), cv::Point2d((int)(start_position[0] * imgCols), (int)(start_position[1] * imgRows)), 25, cv::Scalar(255, 0, 0), -1);
        cv::circle(_images.at(1), cv::Point2d((int)(dest_position[0] * imgCols), (int)(dest_position[1] * imgRows)), 25, cv::Scalar(0, 0, 255), -1);
    }

    float opacity = 0.85;
    cv::addWeighted(_images.at(1), opacity, _images.at(0), 1.0 - opacity, 0, _images.at(2));
}