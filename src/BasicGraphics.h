#ifndef BASICGRAPHICS_H
#define BASICGRAPHICS_H

#include <string>
#include <vector>
#include <opencv2/core.hpp>
#include "BasicIntersection.h"

class BasicGraphics
{
public:
    // constructor / destructor
    BasicGraphics(float minLat, float minLon, float maxLat, float maxLon);

    // getters / setters
    void setBgFilename(std::string filename) { _bgFilename = filename; }
    void setIntersections(std::vector<BasicIntersection> &intersections) { _intersections = intersections; };

    // typical behaviour methods
    void simulate();

private:
    // typical behaviour methods
    void loadBackgroundImg();
    void drawIntersections();

    // member variables
    float minLat, minLon, maxLat, maxLon;
    std::vector<BasicIntersection> _intersections;
    std::string _bgFilename;
    std::string _windowName;
    std::vector<cv::Mat> _images;
};

#endif