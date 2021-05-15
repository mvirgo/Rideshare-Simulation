/**
 * @file route_planner.h
 * @brief Plans a route between two map points using A* Search.
 *
 * @cite Adapted from https://github.com/udacity/CppND-Route-Planning-Project
 *
 */

#ifndef ROUTE_PLANNER_H_
#define ROUTE_PLANNER_H_

#include <iostream>
#include <memory>
#include <mutex>
#include <vector>
#include <string>

#include "mapping/route_model.h"
#include "map_object/map_object.h"

namespace rideshare {

class RoutePlanner {
  public:
    // Constructors / Destructors
    RoutePlanner(RouteModel &model) : model_(model) {};

    // Getters / Setters

    // Primary functionality
    void AStarSearch(std::shared_ptr<MapObject> map_obj);

  private:
    // Route model-related variables
    std::vector<RouteModel::Node*> open_list_;
    RouteModel::Node *start_node_;
    RouteModel::Node *end_node_;

    // Mutex to ensure single access to certain pointers (model, nodes) during A* Search
    std::mutex mtx_;

    // Other variables
    RouteModel &model_;

    // Functions
    // Sort two nodes by h+g value (used by A*)
    static bool Compare(RouteModel::Node* node1, RouteModel::Node* node2);
    // Add all neighbors to a given node
    void AddNeighbors(RouteModel::Node *current_node);
    // Calculate the h-value for a node (distance)
    float CalculateHValue(RouteModel::Node const *node);
    // Construct in reverse the A* Search path, giving start -> finish
    std::vector<Model::Node> ConstructFinalPath(RouteModel::Node *);
    // Get the next node along a given A* Search path
    RouteModel::Node *NextNode();
};

}  // namespace rideshare

#endif  // ROUTE_PLANNER_H_
