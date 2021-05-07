/**
 * @file route_model.cpp
 * @brief Implementation for finding map node neighbors and closest nodes to a point.
 *
 * @cite Adapted from https://github.com/udacity/CppND-Route-Planning-Project
 *
 */

#include "route_model.h"

#include <iostream>

namespace rideshare {

RouteModel::RouteModel(const std::vector<std::byte> &xml) : Model(xml) {
    // Create RouteModel nodes.
    int counter = 0;
    for (Model::Node node : this->Nodes()) {
        nodes_.emplace_back(Node(counter, this, node));
        clean_nodes_.emplace_back(Node(counter, this, node));
        counter++;
    }
    CreateNodeToRoadHashmap();
}


void RouteModel::CreateNodeToRoadHashmap() {
    for (const Model::Road &road : Roads()) {
        for (int node_idx : Ways()[road.way].nodes) {
            if (node_to_road_.find(node_idx) == node_to_road_.end()) {
                node_to_road_[node_idx] = std::vector<const Model::Road *> ();
            }
            node_to_road_[node_idx].push_back(&road);
        }
    }
}


RouteModel::Node *RouteModel::Node::FindNeighbor(std::vector<int> node_indices) {
    Node *closest_node = nullptr;
    Node node;

    for (int node_index : node_indices) {
        node = parent_model->SNodes()[node_index];
        if (this->Distance(node) != 0 && !node.visited_) {
            if (closest_node == nullptr || this->Distance(node) < this->Distance(*closest_node)) {
                closest_node = &parent_model->SNodes()[node_index];
            }
        }
    }
    return closest_node;
}


void RouteModel::Node::FindNeighbors() {
    for (auto & road : parent_model->node_to_road_[this->index_]) {
        RouteModel::Node *new_neighbor = this->FindNeighbor(parent_model->Ways()[road->way].nodes);
        if (new_neighbor) {
            this->neighbors_.emplace_back(new_neighbor);
        }
    }
}


RouteModel::Node &RouteModel::FindClosestNode(const Coordinate &coordinate) {
    Node input;
    input.x = coordinate.x;
    input.y = coordinate.y;

    float min_dist = std::numeric_limits<float>::max();
    float dist;
    int closest_idx;

    for (const Model::Road &road : Roads()) {
        for (int node_idx : Ways()[road.way].nodes) {
            dist = input.Distance(SNodes()[node_idx]);
            if (dist < min_dist) {
                closest_idx = node_idx;
                min_dist = dist;
            }
        }
    }

    return SNodes()[closest_idx];
}

}  // namespace rideshare
