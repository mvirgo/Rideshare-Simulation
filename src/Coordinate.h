#ifndef COORDINATE_H
#define COORDINATE_H

namespace rideshare {

struct Coordinate {
    double x;
    double y;

    bool operator==(const Coordinate &other_coord) const {
        return x == other_coord.x && y == other_coord.y;
    }
};

}  // namespace rideshare

#endif
