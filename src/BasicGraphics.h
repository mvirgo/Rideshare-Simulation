#ifndef BASICGRAPHICS_H
#define BASICGRAPHICS_H

#include <string>
#include <vector>
#include <opencv2/core.hpp>
#include "PassengerQueue.h"
#include "VehicleManager.h"

class BasicGraphics {
  public:
    // constructor / destructor
    BasicGraphics(float min_lat, float min_lon, float max_lat, float max_lon);

    // getters / setters
    void SetBgFilename(std::string filename) { bgFilename_ = filename; }
    void SetVehicles(const std::shared_ptr<VehicleManager> &vehicle_manager) { vehicle_manager_ = vehicle_manager; }
    void SetPassengers(const std::shared_ptr<PassengerQueue> &passenger_queue) { passenger_queue_ = passenger_queue; }

    // typical behaviour methods
    void Simulate();

  private:
    // typical behaviour methods
    void LoadBackgroundImg();
    void DrawSimulation();
    void DrawPassengers(float img_rows, float img_cols);
    void DrawPassenger(float img_rows, float img_cols, int marker_size, const std::shared_ptr<Passenger> &passenger);
    void DrawVehicles(float img_rows, float img_cols);

    // member variables
    float min_lat_, min_lon_, max_lat_, max_lon_;
    std::shared_ptr<VehicleManager> vehicle_manager_;
    std::shared_ptr<PassengerQueue> passenger_queue_;
    std::string bgFilename_;
    std::string windowName_;
    std::vector<cv::Mat> images_;
};

#endif