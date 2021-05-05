#include <cstdlib>
#include <ctime>
#include <optional>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <vector>
#include "RouteModel.h"
#include "BasicGraphics.h"
#include "PassengerQueue.h"
#include "RideMatcher.h"
#include "RoutePlanner.h"
#include "VehicleManager.h"

static std::optional<std::vector<std::byte>> ReadFile(const std::string &path) {   
    std::ifstream is{path, std::ios::binary | std::ios::ate};
    if ( !is )
        return std::nullopt;
    
    auto size = is.tellg();
    std::vector<std::byte> contents(size);    
    
    is.seekg(0);
    is.read((char*)contents.data(), size);

    if ( contents.empty() )
        return std::nullopt;
    return std::move(contents);
}

int main() {
    std::string location = "downtown-kc";
    std::string osm_data_file = "../data/" + location + ".osm";

    std::vector<std::byte> osm_data;
 
    if ( osm_data.empty() && !osm_data_file.empty() ) {
        std::cout << "Reading OpenStreetMap data from the following file: " <<  osm_data_file << std::endl;
        auto data = ReadFile(osm_data_file);
        if ( !data ) {
            std::cout << "Failed to read." << std::endl;
        } else {
            osm_data = std::move(*data);
        }
    }

    RouteModel model{osm_data};

    srand((unsigned) time(NULL)); // Seed random number generator

    // Create a shared route planner
    std::shared_ptr<RoutePlanner> route_planner = std::make_shared<RoutePlanner>(model);

    // Create vehicles
    std::shared_ptr<VehicleManager> vehicles = std::make_shared<VehicleManager>(&model, route_planner);

    // Create passenger queue
    std::shared_ptr<PassengerQueue> passengers = std::make_shared<PassengerQueue>(&model, route_planner);

    // Create the ride matcher
    std::shared_ptr<RideMatcher> ride_matcher = std::make_shared<RideMatcher>(passengers, vehicles);

    // Attach ride matcher to the other two
    vehicles->SetRideMatcher(ride_matcher);
    passengers->SetRideMatcher(ride_matcher);

    // Start the simulations
    ride_matcher->Simulate();
    vehicles->Simulate();
    passengers->Simulate();

    // Draw the map
    BasicGraphics *graphics = new BasicGraphics(model.MinLat(), model.MinLon(), model.MaxLat(), model.MaxLon());
    std::string background_img = "../data/" + location + ".png";
    graphics->SetBgFilename(background_img);
    graphics->SetPassengers(passengers);
    graphics->SetVehicles(vehicles);
    graphics->Simulate();

    return 0;
}