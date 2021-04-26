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
#include "BasicIntersection.h"
#include "PassengerQueue.h"
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

    // Finding shared nodes between roads
    std::set<int> all_nodes;
    std::vector<int> shared_nodes;
    for (auto road : model.Roads()) {
        auto way = model.Ways()[road.way];
        for (auto node : way.nodes) {
            if (all_nodes.count(node) != 1) {
                // Not found
                all_nodes.insert(node);
            } else {
                shared_nodes.push_back(node);
            }
        }
    }
    std::cout << "Shared road nodes total: " << shared_nodes.size() << std::endl;

    // Find & create all intersections
    int num_intersections = 0;
    std::vector<BasicIntersection> intersections;
    BasicIntersection current_intersection;
    for (int id : shared_nodes) {
        num_intersections++;
        auto shared_node = model.Nodes()[id];
        current_intersection.SetPosition(shared_node.x, shared_node.y);
        intersections.push_back(current_intersection);
    }
    std::cout << "Total intersections: " << num_intersections << std::endl;

    // Create initial vehicles
    // TODO: Change/remove when using full simulation
    VehicleManager vehicles = VehicleManager(&model);

    // Create basic passenger queue
    // TODO: Remove when using full simulation
    PassengerQueue passengers = PassengerQueue(&model);

    // Draw the map
    BasicGraphics *graphics = new BasicGraphics(model.MinLat(), model.MinLon(), model.MaxLat(), model.MaxLon());
    std::string background_img = "../data/" + location + ".png";
    graphics->SetBgFilename(background_img);
    graphics->SetIntersections(intersections);
    graphics->SetPassengers(passengers);
    graphics->SetVehicles(vehicles);
    graphics->Simulate();

    return 0;
}