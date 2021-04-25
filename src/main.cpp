#include <optional>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <vector>
#include "RouteModel.h"
#include "BasicGraphics.h"
#include "BasicIntersection.h"

static std::optional<std::vector<std::byte>> ReadFile(const std::string &path)
{   
    std::ifstream is{path, std::ios::binary | std::ios::ate};
    if( !is )
        return std::nullopt;
    
    auto size = is.tellg();
    std::vector<std::byte> contents(size);    
    
    is.seekg(0);
    is.read((char*)contents.data(), size);

    if( contents.empty() )
        return std::nullopt;
    return std::move(contents);
}

int main() {
    std::string location = "downtown-kc";
    std::string osm_data_file = "../data/" + location + ".osm";

    std::vector<std::byte> osm_data;
 
    if( osm_data.empty() && !osm_data_file.empty() ) {
        std::cout << "Reading OpenStreetMap data from the following file: " <<  osm_data_file << std::endl;
        auto data = ReadFile(osm_data_file);
        if ( !data ) {
            std::cout << "Failed to read." << std::endl;
        } else {
            osm_data = std::move(*data);
        }
    }

    RouteModel model{osm_data};

    // Printing out basic stats
    std::cout << "Model nodes: " << model.Nodes().size() << std::endl;
    std::cout << "Model roads: " << model.Roads().size() << std::endl;
    std::cout << "Model ways: " << model.Ways().size() << std::endl;
    // Printing nodes for a single road
    // auto roadToPrint = model.Roads().front();
    // std::cout << "Road type: " << roadToPrint.type << std::endl;;
    // std::cout << "Road way: " << roadToPrint.way << std::endl;;
    // auto wayToPrint = model.Ways()[roadToPrint.way];
    // std::cout << "Road way's nodes: " << wayToPrint.nodes.size() << std::endl;
    // for (int i = 0; i < wayToPrint.nodes.size(); ++i) {
    //     std::cout << "Node " << i << ": " << wayToPrint.nodes.at(i) << std::endl;
    //     auto nodeToPrint = model.Nodes()[wayToPrint.nodes.at(i)];
    //     std::cout << "Node's x: " << nodeToPrint.x << std::endl;
    //     std::cout << "Node's y: " << nodeToPrint.y << std::endl;
    // }
    //std::cout << model.Nodes().front().x << std::endl;
    //std::cout << model.Nodes().front().y << std::endl;
    //std::cout << model.Roads().front().type << std::endl;
    //std::cout << model.Roads().front().way << std::endl;
    //std::cout << model.Ways().front().nodes.front() << std::endl;
    //std::cout << model.Ways().front().nodes.size() << std::endl;

    // Try to find bottom & top left/right points
    // double smallest = -1000;
    // double largest = 1000;
    // std::vector<double> tl = {largest,smallest};
    // std::vector<double> tr = {smallest,smallest};
    // std::vector<double> bl = {largest,largest};
    // std::vector<double> br = {smallest,largest};

    // for (auto node: model.Nodes()) {
    //     if (node.x <= tl[0] && node.y >= tl[1]) { // lower or equal x, higher or equal y
    //         tl = {node.x, node.y};
    //     }
    //     if (node.x >= tr[0] && node.y >= tr[1]) { // higher or equal x, higher or equal y
    //         tr = {node.x, node.y};
    //     }
    //     if (node.x <= bl[0] && node.y <= bl[1]) { // lower or equal x, lower or equal y
    //         bl = {node.x, node.y};
    //     }
    //     if (node.x >= br[0] && node.y <= br[1]) { // higher or equal x, lower or equal y
    //         br = {node.x, node.y};
    //     }
    // }

    // std::cout << "The outer most points are: " << std::endl;
    // std::cout << "Top left: " << tl[0] << " " << tl[1] << std::endl;
    // std::cout << "Top right: " << tr[0] << " " << tr[1] << std::endl;
    // std::cout << "Bottom left: " << bl[0] << " " << bl[1] << std::endl;
    // std::cout << "Bottom right: " << br[0] << " " << br[1] << std::endl;

    // // Find the outermost points only in roads
    // tl = {largest,smallest};
    // tr = {smallest,smallest};
    // bl = {largest,largest};
    // br = {smallest,largest};

    // for (auto road : model.Roads()) {
    //     auto way = model.Ways()[road.way];
    //     for (auto nodeId : way.nodes) {
    //         auto node = model.Nodes()[nodeId];
    //         if (node.x <= tl[0] && node.y >= tl[1]) { // lower or equal x, higher or equal y
    //             tl = {node.x, node.y};
    //         }
    //         if (node.x >= tr[0] && node.y >= tr[1]) { // higher or equal x, higher or equal y
    //             tr = {node.x, node.y};
    //         }
    //         if (node.x <= bl[0] && node.y <= bl[1]) { // lower or equal x, lower or equal y
    //             bl = {node.x, node.y};
    //         }
    //         if (node.x >= br[0] && node.y <= br[1]) { // higher or equal x, lower or equal y
    //             br = {node.x, node.y};
    //         }
    //     }
    // }

    // std::cout << "The outer most points on roads only are: " << std::endl;
    // std::cout << "Top left: " << tl[0] << " " << tl[1] << std::endl;
    // std::cout << "Top right: " << tr[0] << " " << tr[1] << std::endl;
    // std::cout << "Bottom left: " << bl[0] << " " << bl[1] << std::endl;
    // std::cout << "Bottom right: " << br[0] << " " << br[1] << std::endl;

    // Find road with least and most nodes
    int mostNodes = 0;
    int mostCount = 0;
    int leastNodes = 100;
    int leastCount = 0;
    int nodeCount = 0;
    for (auto road : model.Roads()) {
        auto way = model.Ways()[road.way];
        if (way.nodes.size() > mostNodes) {
            mostNodes = way.nodes.size();
            mostCount = 1;
        } else if (way.nodes.size() == mostNodes) {
            mostCount++;
        }
        if (way.nodes.size() < leastNodes) {
            leastNodes = way.nodes.size();
            mostCount = 1;
        } else if (way.nodes.size() == leastNodes) {
            leastCount++;
        }
        nodeCount += way.nodes.size();
    }

    std::cout << "Road with most nodes has: " << mostNodes << std::endl;
    std::cout << mostCount << " roads had that many nodes." << std::endl;
    std::cout << "Road with least nodes has: " << leastNodes << std::endl;
    std::cout << leastCount << " roads had that many nodes." << std::endl;
    std::cout << "There were this many nodes on roads: " << nodeCount << std::endl;

    // Finding shared nodes between roads
    std::set<int> allNodes;
    std::vector<int> sharedNodes;
    for (auto road : model.Roads()) {
        auto way = model.Ways()[road.way];
        for (auto node : way.nodes) {
            if (allNodes.count(node) != 1) {
                // Not found
                allNodes.insert(node);
            } else {
                sharedNodes.push_back(node);
            }
        }
    }

    std::cout << "Shared road nodes total: " << sharedNodes.size() << std::endl;

    // List all neighbors of a node
    int nodeNum = 0;
    RouteModel::Node node;
    while (true) {
        node = model.SNodes()[nodeNum];
        node.FindNeighbors();
        if (node.neighbors.size() > 2) {
            break;
        } else {
            nodeNum++;
        }
    }
    auto neighbors = node.neighbors;
    std::cout << "Original node at: " << node.x << " " << node.y << std::endl;
    for (auto neighbor : neighbors) {
        std::cout << "Neighbor at: " << neighbor->x << " " << neighbor->y << std::endl;
    }

    // Find & create all intersections
    int numIntersections = 0;
    std::vector<BasicIntersection> intersections;
    BasicIntersection currentIntersection;
    // for (auto road : model.Roads()) {
    //     // Remove any "stub" roads
    //     if (model.Ways()[road.way].nodes.size() <= 2) {
    //         continue;
    //     }
    //     for (auto nodeNum : model.Ways()[road.way].nodes) {
    //         node = model.SNodes()[nodeNum];
    //         node.FindNeighbors();
    //         if (node.neighbors.size() >= 2) { // is an intersection
    //             numIntersections++;
    //             currentIntersection.setPosition(node.x, node.y);
    //             intersections.push_back(currentIntersection);
    //         }
    //     }
    // }
    for (int id : sharedNodes) {
        numIntersections++;
        auto sharedNode = model.Nodes()[id];
        currentIntersection.setPosition(sharedNode.x, sharedNode.y);
        intersections.push_back(currentIntersection);
    }
    std::cout << "Total intersections: " << numIntersections << std::endl;
    // std::vector<BasicIntersection> intersections;
    // BasicIntersection currentIntersection;
    // for (auto node : model.SNodes()) {
    //     currentIntersection.setPosition(node.x, node.y);
    //     intersections.push_back(currentIntersection);
    // }

    // Draw the intersections
    BasicGraphics *graphics = new BasicGraphics(model.MinLat(), model.MinLon(), model.MaxLat(), model.MaxLon());
    std::string backgroundImg = "../data/" + location + ".png";
    graphics->setBgFilename(backgroundImg);
    graphics->setIntersections(intersections);
    graphics->simulate();

    return 0;
}