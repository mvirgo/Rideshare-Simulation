#ifndef ROUTE_PLANNER_H
#define ROUTE_PLANNER_H

#include <iostream>
#include <vector>
#include <string>
#include "RouteModel.h"
#include "Vehicle.h"


class RoutePlanner {
  public:
    // Constructors / Destructors
    RoutePlanner(RouteModel &model) : model_(model) {};

    // Getters / Setters

    // Primary functionality
    void AStarSearch(Vehicle &vehicle);

  private:
    // Route model-related variables
    std::vector<RouteModel::Node*> open_list_;
    RouteModel::Node *start_node_;
    RouteModel::Node *end_node_;

    // Other variables
    RouteModel &model_;

    // Functions
    static bool Compare(RouteModel::Node* node1, RouteModel::Node* node2);
    void AddNeighbors(RouteModel::Node *current_node);
    float CalculateHValue(RouteModel::Node const *node);
    std::vector<RouteModel::Node> ConstructFinalPath(RouteModel::Node *);
    RouteModel::Node *NextNode();
};

#endif