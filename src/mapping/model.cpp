/**
 * @file model.cpp
 * @brief Implementation to read and store OpenStreetMap data, as well as getting a random map position.
 *
 * @cite Adapted from https://github.com/udacity/CppND-Route-Planning-Project
 *
 */

#include "model.h"

#include <iostream>
#include <string_view>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <assert.h>

#include "pugixml.hpp"

#include "coordinate.h"

namespace rideshare {

// Only need road types (and no footways)
static Model::Road::Type String2RoadType(std::string_view type) {
    if( type == "motorway" )        return Model::Road::Motorway;
    if( type == "trunk" )           return Model::Road::Trunk;
    if( type == "primary" )         return Model::Road::Primary;
    if( type == "secondary" )       return Model::Road::Secondary;    
    if( type == "tertiary" )        return Model::Road::Tertiary;
    if( type == "residential" )     return Model::Road::Residential;
    if( type == "living_street" )   return Model::Road::Residential;
    return Model::Road::Invalid; // don't want other road types
}

Model::Model(const std::vector<std::byte> &xml) {
    
    LoadData(xml);

    std::sort(roads_.begin(), roads_.end(), [](const auto &_1st, const auto &_2nd) {
        return (int)_1st.type < (int)_2nd.type; 
    });
}

Coordinate Model::GetRandomMapPosition() const noexcept {
    // Get float values as percentages of map to use
    float randPercentageLon = (float) rand() / RAND_MAX;
    float randPercentageLat = (float) rand() / RAND_MAX;
    return (Coordinate){ .x = ((max_lon_ - min_lon_) * randPercentageLon) + min_lon_,
                         .y = ((max_lat_ - min_lat_) * randPercentageLat) + min_lat_ };
}

void Model::LoadData(const std::vector<std::byte> &xml) {
    using namespace pugi;
    
    xml_document doc;
    if ( !doc.load_buffer(xml.data(), xml.size()) )
        throw std::logic_error("failed to parse the xml file");
    
    if ( auto bounds = doc.select_nodes("/osm/bounds"); !bounds.empty() ) {
        auto node = bounds.first().node();
        min_lat_ = atof(node.attribute("minlat").as_string());
        max_lat_ = atof(node.attribute("maxlat").as_string());
        min_lon_ = atof(node.attribute("minlon").as_string());
        max_lon_ = atof(node.attribute("maxlon").as_string());
    } else {
        throw std::logic_error("map's bounds are not defined");
    }

    std::unordered_map<std::string, int> node_id_to_num;
    for ( const auto &node: doc.select_nodes("/osm/node") ) {
        node_id_to_num[node.node().attribute("id").as_string()] = (int)nodes_.size();
        nodes_.emplace_back();        
        nodes_.back().y = atof(node.node().attribute("lat").as_string());
        nodes_.back().x = atof(node.node().attribute("lon").as_string());
    }

    std::unordered_map<std::string, int> way_id_to_num;    
    for ( const auto &way: doc.select_nodes("/osm/way") ) {
        auto node = way.node();
        
        const auto way_num = (int)ways_.size();
        way_id_to_num[node.attribute("id").as_string()] = way_num;
        ways_.emplace_back();
        auto &new_way = ways_.back();
        
        for ( auto child: node.children() ) {
            auto name = std::string_view{child.name()}; 
            if ( name == "nd" ) {
                auto ref = child.attribute("ref").as_string();
                if ( auto it = node_id_to_num.find(ref); it != end(node_id_to_num) )
                    new_way.nodes.emplace_back(it->second);
            } else if( name == "tag" ) {
                auto category = std::string_view{child.attribute("k").as_string()};
                auto type = std::string_view{child.attribute("v").as_string()};
                if ( category == "highway" ) {
                    if ( auto road_type = String2RoadType(type); road_type != Road::Invalid ) {
                        roads_.emplace_back();
                        roads_.back().way = way_num;
                        roads_.back().type = road_type;
                    }
                }
            }
        }
    }
}

}  // namespace rideshare
