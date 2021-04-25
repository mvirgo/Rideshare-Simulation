#ifndef MODEL_H
#define MODEL_H

/*
  The below has been adapted from https://github.com/udacity/CppND-Route-Planning-Project from Udacity
*/

#include <vector>
#include <unordered_map>
#include <string>
#include <cstddef>

class Model
{
public:
    struct Node {
        double x = 0.f;
        double y = 0.f;
    };
    
    struct Way {
        std::vector<int> nodes;
    };
    
    struct Road {
        enum Type { Invalid, Unclassified, Service, Residential,
            Tertiary, Secondary, Primary, Trunk, Motorway };
        int way;
        Type type;
    };  
    
    Model( const std::vector<std::byte> &xml );
    
    auto MetricScale() const noexcept { return m_MetricScale; }    
    
    auto &Nodes() const noexcept { return m_Nodes; }
    auto &Ways() const noexcept { return m_Ways; }
    auto &Roads() const noexcept { return m_Roads; }
    auto &MinLat() const noexcept { return m_MinLat; }
    auto &MaxLat() const noexcept { return m_MaxLat; }
    auto &MinLon() const noexcept { return m_MinLon; }
    auto &MaxLon() const noexcept { return m_MaxLon; }

    std::vector<double> GetRandomMapPosition();
    
private:
    void AdjustCoordinates();
    void LoadData(const std::vector<std::byte> &xml);
    
    std::vector<Node> m_Nodes;
    std::vector<Way> m_Ways;
    std::vector<Road> m_Roads;
    
    double m_MinLat = 0.;
    double m_MaxLat = 0.;
    double m_MinLon = 0.;
    double m_MaxLon = 0.;
    double m_MetricScale = 1.f;
};

#endif