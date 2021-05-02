# Ridesharing Simulation with C++

My Capstone project for the C++ Nanodegree at Udacity, a rideshare simulator. It extends the concurrency project based on a traffic simulation, as well as taking in parts of the earlier route planning project, in order to simulate ridesharing apps that are able to pick up passengers. It also makes use of map data from [OpenStreetMap](https://www.openstreetmap.org/), primarily a portion of Downtown Kansas City, but also with an additional data file from Paris, France, which can be switched to in `main.cpp` to show generalization to another map.

Squares on the map are vehicles, while the "diamonds" are passengers and the X's their final destinations.

The simulator currently defaults to always having 10 vehicles, as well as maxing out to 10 passengers at most awaiting rides (meaning there can be at most 10 passengers in vehicles and 10 waiting, or 20 total).

## Future Improvement Areas

1. The current matching mechanism just matches the earliest vehicle and passenger that requested the ride matcher to get them a match. This can be improved to instead match the nearest empty vehicle to the passenger.
2. Passengers currently "teleport" to the vehicle when the vehicle reaches the closest node on the road to the passenger location, and also teleport out when similarly reaching the closest road node to the final destination. This can be improved to show the passenger "walk" to and from the vehicle instead.
3. A vehicle or passenger that is "stuck" due to no way to get to them or get to a destination may later be removed after a period of time.
4. Vehicles currently ignore the directions of streets. "Fixing" this may cause more situations where a vehicle or passenger is "stuck".
5. Further outputs will be added to the console so more information is given about what is occurring.

## Dependencies for Running Locally
* cmake >= 3.11
  * All OSes: [click here for installation instructions](https://cmake.org/install/)
* make >= 4.1 (Linux, Mac), 3.81 (Windows)
  * Linux: make is installed by default on most Linux distros
  * Mac: [install Xcode command line tools to get make](https://developer.apple.com/xcode/features/)
  * Windows: [Click here for installation instructions](http://gnuwin32.sourceforge.net/packages/make.htm)
* OpenCV >= 4.1
  * The OpenCV 4.1.0 source code can be found [here](https://github.com/opencv/opencv/tree/4.1.0)
* gcc/g++ >= 5.4
  * Linux: gcc / g++ is installed by default on most Linux distros
  * Mac: same deal as make - [install Xcode command line tools](https://developer.apple.com/xcode/features/)
  * Windows: recommend using [MinGW](http://www.mingw.org/)

## Basic Build Instructions

1. Clone this repo.
2. Make a build directory in the top level directory: `mkdir build && cd build`
3. Compile: `cmake .. && make`
4. Run it: `./rideshare_simulation`

## Known Issues
 - The current version of the simulator will eventually crash if a vehicle tries to reach an unreachable passenger or destination.
