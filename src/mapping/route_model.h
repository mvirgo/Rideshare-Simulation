/**
 * @file route_model.h
 * @brief Child of model.h used for processing and storing information needed for A* Search.
 *
 * @cite Adapted from https://github.com/udacity/CppND-Route-Planning-Project
 *
 */

#ifndef ROUTE_MODEL_H_
#define ROUTE_MODEL_H_

#include <cmath>
#include <limits>
#include <iostream>
#include <unordered_map>

#include "coordinate.h"
#include "model.h"

namespace rideshare {

class RouteModel : public Model {

  public:
    class Node : public Model::Node {
      public:
        Node * parent_ = nullptr;
        float h_value_ = std::numeric_limits<float>::max();
        float g_value_ = 0.0;
        bool visited_ = false;
        std::vector<Node *> neighbors_;

        // Find neighbors of nodes
        void FindNeighbors();
        // Find distance between two nodes
        float Distance(Node other) const {
            return std::sqrt(std::pow((x - other.x), 2) + std::pow((y - other.y), 2));
        }

        // Constructors
        Node(){}
        Node(int idx, RouteModel * search_model, Model::Node node) : Model::Node(node), parent_model(search_model), index_(idx) {}

      private:
        int index_;
        // Find neighbor of given node
        Node * FindNeighbor(std::vector<int> node_indices);
        RouteModel * parent_model = nullptr;
    };

    // Constructor
    RouteModel(const std::vector<std::byte> &xml);
    // Getter
    auto &SNodes() { return nodes_; }
    // Find closest road node to a coordinate
    Node &FindClosestNode(const Coordinate &coordinate);
    // Reset all nodes so can be re-used by route planner
    void ResetNodes() { nodes_ = clean_nodes_; }
    
  private:
    // Map nodes to given roads on the map
    void CreateNodeToRoadHashmap();
    std::unordered_map<int, std::vector<const Model::Road *>> node_to_road_;
    std::vector<Node> nodes_;
    std::vector<Node> clean_nodes_;

};

}  // namespace rideshare

#endif  // ROUTE_MODEL_H_
