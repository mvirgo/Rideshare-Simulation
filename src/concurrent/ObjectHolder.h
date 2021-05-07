#ifndef OBJECT_HOLDER_H_
#define OBJECT_HOLDER_H_

#include "mapping/RouteModel.h"
#include "routing/RoutePlanner.h"

namespace rideshare {

class ObjectHolder {
  public:
    // Constructor / Destructor
    ObjectHolder() {};
    ObjectHolder(RouteModel *model, std::shared_ptr<RoutePlanner> route_planner) : model_(model), route_planner_(route_planner) {};

  protected:
    virtual void GenerateNew() {};
    int MAX_OBJECTS = 10; // TODO: Change or remove limit?
    RouteModel *model_;
    int idCnt_ = 0; // Count object ids
    std::shared_ptr<RoutePlanner> route_planner_; // Route planner to use throughout the sim
};

}  // namespace rideshare

#endif  // OBJECT_HOLDER_H_
