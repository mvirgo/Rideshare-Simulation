#ifndef ROUTE_PLANNER_H
#define ROUTE_PLANNER_H

#include <iostream>
#include <mutex>
#include <vector>
#include <string>
#include "RouteModel.h"
#include "MapObject.h"


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
    static bool Compare(RouteModel::Node* node1, RouteModel::Node* node2);
    void AddNeighbors(RouteModel::Node *current_node);
    float CalculateHValue(RouteModel::Node const *node);
    std::vector<Model::Node> ConstructFinalPath(RouteModel::Node *);
    RouteModel::Node *NextNode();
};

#endif