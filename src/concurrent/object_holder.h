/**
 * @file object_holder.h
 * @brief Parent for classes generating, holding and routing about vehicles/passengers.
 *
 * @copyright Copyright (c) 2021, Michael Virgo, released under the MIT License.
 *
 */

#ifndef OBJECT_HOLDER_H_
#define OBJECT_HOLDER_H_

#include <memory>

#include "mapping/route_model.h"
#include "routing/route_planner.h"

namespace rideshare {

class ObjectHolder {
  public:
    // Constructor / Destructor
    ObjectHolder(RouteModel *model, std::shared_ptr<RoutePlanner> route_planner,
                 int max_objects) :
      model_(model), route_planner_(route_planner), MAX_OBJECTS_(max_objects) {};

  protected:
    virtual void GenerateNew() {};
    const int MAX_OBJECTS_; // Set max number of objects to pause generation at
    RouteModel *model_;
    int idCnt_ = 0; // Count object ids
    std::shared_ptr<RoutePlanner> route_planner_; // Route planner to use throughout the sim
};

}  // namespace rideshare

#endif  // OBJECT_HOLDER_H_
