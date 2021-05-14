/**
 * @file graphics.h
 * @brief Draw vehicles and passengers on a map image.
 *
 * @cite Adapted from https://github.com/udacity/CppND-Program-a-Concurrent-Traffic-Simulation
 * @copyright Copyright (c) 2021, Michael Virgo, released under the MIT License.
 *
 */

#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include <string>
#include <vector>
#include <opencv2/core.hpp>

#include "concurrent/passenger_queue.h"
#include "concurrent/vehicle_manager.h"
#include "map_object/passenger.h"

namespace rideshare {

class Graphics {
  public:
    // Constructor
    Graphics(float min_lat, float min_lon, float max_lat, float max_lon);

    // Setters
    void SetBgFilename(std::string filename) { bgFilename_ = filename; }
    void SetVehicles(const std::shared_ptr<VehicleManager> &vehicle_manager) { vehicle_manager_ = vehicle_manager; }
    void SetPassengers(const std::shared_ptr<PassengerQueue> &passenger_queue) { passenger_queue_ = passenger_queue; }

    // Concurrent drawing simulation
    void Simulate();

  private:
    // Load the given OSM tile image
    void LoadBackgroundImg();
    // Loop to draw desired objects on OSM tile
    void DrawSimulation();
    // Draw waiting passengers onto the image
    void DrawPassengers(float img_rows, float img_cols);
    // Draw a single passenger (position and destination) on the image
    void DrawPassenger(float img_rows, float img_cols, int marker_size, const std::shared_ptr<Passenger> &passenger);
    // Draw all vehicles on the image
    void DrawVehicles(float img_rows, float img_cols);

    // Member variables
    float min_lat_, min_lon_, max_lat_, max_lon_;
    std::shared_ptr<VehicleManager> vehicle_manager_;
    std::shared_ptr<PassengerQueue> passenger_queue_;
    std::string bgFilename_;
    std::string windowName_;
    std::vector<cv::Mat> images_;
};

}  // namespace rideshare

#endif  // GRAPHICS_H_
