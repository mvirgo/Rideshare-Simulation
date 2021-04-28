/*
  The below has been adapted from https://github.com/udacity/CppND-Route-Planning-Project from Udacity,
    along with my solution to that project.
*/

#include "RoutePlanner.h"
#include <algorithm>
#include "RouteModel.h"
#include "Vehicle.h"

RoutePlanner::RoutePlanner(RouteModel &model, Vehicle &vehicle): model_(model), vehicle_(vehicle) {
    auto start_pos = vehicle_.GetPosition();
    auto dest_pos = vehicle_.GetDestination();

    // Use FindClosestNode to find the closest nodes to the starting and ending coordinates.
    //  and store the nodes found
    this->start_node_ = &model_.FindClosestNode(start_pos[0], start_pos[1]);
    this->end_node_ = &model_.FindClosestNode(dest_pos[0], dest_pos[1]);
}

// Calculate H Value (in this case, distance) for A* Search
float RoutePlanner::CalculateHValue(RouteModel::Node const *node) {
    return node->Distance(*end_node_);
}

// Expand the current node by adding all unvisited neighbors to the open list
void RoutePlanner::AddNeighbors(RouteModel::Node *current_node) {
    // Find all of node's neighbors
    current_node->FindNeighbors();
    // Loop through all neighbors
    for (auto node: current_node->neighbors_) {
        // Set parent, h_value and g_value
        node->parent_ = current_node;
        node->h_value_ = CalculateHValue(node);
        node->g_value_ = current_node->g_value_ + node->Distance(*current_node); // Current node g + how far from current
        // Add to open list and mark as visited
        this->open_list_.emplace_back(node);
        node->visited_ = true;
    }
}

// Helper function for NextNode to sort
bool RoutePlanner::Compare(RouteModel::Node* node1, RouteModel::Node* node2) {
    // Compare by h+g values
    return (node1->g_value_ + node1->h_value_) > (node2->g_value_ + node2->h_value_);
}

// Get next node (lowest sum) in open list
RouteModel::Node *RoutePlanner::NextNode() {
    // Sort the nodes in open_list
    std::sort(open_list_.begin(), open_list_.end(), Compare);
    // Grab the lowest sum pointer from the back of open_list, then remove it
    auto current = open_list_.back();
    open_list_.pop_back();

    return current;
}

// Construct a final path based on result of A* Search
std::vector<RouteModel::Node> RoutePlanner::ConstructFinalPath(RouteModel::Node *current_node) {
    // Create path_found vector
    std::vector<RouteModel::Node> path_found;
    
    // Iterate until a node has no parent
    RouteModel::Node follow_node = *current_node;
    path_found.emplace_back(follow_node); // Add initial node to path_found
    while (follow_node.parent_ != nullptr) {
        // Update the follow_node to the parent
        follow_node = *(follow_node.parent_);
        // Add the node to path_found
        path_found.emplace_back(follow_node);
    }

    // Reverse the path_found for proper ordering
    std::reverse(path_found.begin(), path_found.end());

    return path_found;

}

// A* Search Algorithm
// TODO: Handle if route not found?
void RoutePlanner::AStarSearch() {
    RouteModel::Node *current_node = nullptr;

    // Add start node to open list
    start_node_->visited_ = true;
    open_list_.emplace_back(start_node_);

    // Loop while not at goal and can expand nodes
    while (open_list_.size() > 0) {
        // Get the next node
        current_node = NextNode();
        // Check if at the goal state, and if so, construct the final path
        if (current_node->x == end_node_->x && current_node->y == end_node_->y) {
            vehicle_.SetPath(ConstructFinalPath(current_node));
            break; // Can stop searching
        }
        // Add all neighbors for current node
        AddNeighbors(current_node);
    }
}