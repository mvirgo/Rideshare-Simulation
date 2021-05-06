#ifndef ROUTE_MODEL_H
#define ROUTE_MODEL_H

#include <cmath>
#include <limits>
#include <iostream>
#include <unordered_map>

#include "Coordinate.h"
#include "Model.h"

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

        void FindNeighbors();
        float Distance(Node other) const {
            return std::sqrt(std::pow((x - other.x), 2) + std::pow((y - other.y), 2));
        }

        Node(){}
        Node(int idx, RouteModel * search_model, Model::Node node) : Model::Node(node), parent_model(search_model), index_(idx) {}

      private:
        int index_;
        Node * FindNeighbor(std::vector<int> node_indices);
        RouteModel * parent_model = nullptr;
    };

    RouteModel(const std::vector<std::byte> &xml);
    Node &FindClosestNode(const Coordinate &coordinate);
    auto &SNodes() { return nodes_; }
    void ResetNodes() { nodes_ = clean_nodes_; }
    
  private:
    void CreateNodeToRoadHashmap();
    std::unordered_map<int, std::vector<const Model::Road *>> node_to_road_;
    std::vector<Node> nodes_;
    std::vector<Node> clean_nodes_;

};

}  // namespace rideshare

#endif
