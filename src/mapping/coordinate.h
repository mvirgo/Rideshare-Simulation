/**
 * @file coordinate.h
 * @brief Store x & y coordinate information, along with comparing equality.
 *
 * @copyright Copyright (c) 2021, Michael Virgo, released under the MIT License.
 *
 */

#ifndef COORDINATE_H_
#define COORDINATE_H_

namespace rideshare {

struct Coordinate {
    double x;
    double y;

    bool operator==(const Coordinate &other_coord) const {
        return x == other_coord.x && y == other_coord.y;
    }
};

}  // namespace rideshare

#endif  // COORDINATE_H_
