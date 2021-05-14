# Ridesharing Simulation with C++

My Capstone project for the C++ Nanodegree at Udacity, a rideshare simulator. It extends the concurrency project based on a traffic simulation, as well as taking in parts of the earlier route planning project, in order to simulate ridesharing apps that are able to pick up passengers. It also makes use of map data from [OpenStreetMap](https://www.openstreetmap.org/), primarily a portion of Downtown Kansas City, but also with an additional data file from Paris, France, which can be switched to in `main.cpp` to show generalization to another map.

Squares on the map are vehicles, while the "diamonds" are passengers and the X's their final destinations.

The simulator currently defaults to always having 10 vehicles, as well as maxing out to 10 passengers at most awaiting rides (meaning there can be at most 10 passengers in vehicles and 10 waiting, or 20 total). When a vehicle drops off due to being stuck, it is immediately replaced; new passengers are added every 3-5 seconds if the queue isn't full.

The above amounts were somewhat arbitrarily chosen to make the simulation easier to follow visually, but the program is capable of using much higher amounts of each with a shorter passenger generation time.

## Examples

Below, you can see an example of the simulation running on the Downtown KC map with 10 vehicles and up to 10 passengers waiting at any given time. (You can click to watch a video.)

[![KC Map 10](https://img.youtube.com/vi/KivuGvkSRUI/sddefault.jpg)](https://www.youtube.com/watch?v=KivuGvkSRUI)

The next example below is to show that the simulator can generalize to other maps, in this case the area near the Arc de Triomphe in Paris.

[![Paris Map 10](https://img.youtube.com/vi/IibIyyhbfHs/sddefault.jpg)](https://www.youtube.com/watch?v=IibIyyhbfHs)

This final example is to show the simulation can scale up fairly well - 100 vehicles, up to 100 passengers waiting for pick-up, with generation of a new waiting passenger happening every 0-1 second.

[![KC Map 100](https://img.youtube.com/vi/0u3_8vQH2Xo/sddefault.jpg)](https://www.youtube.com/watch?v=0u3_8vQH2Xo)

## Future Improvement Areas

1. Passengers currently "teleport" to the vehicle when the vehicle reaches the closest node on the road to the passenger location, and also teleport out when similarly reaching the closest road node to the final destination. This can be improved to show the passenger "walk" to and from the vehicle instead.
2. Vehicles currently ignore the directions of streets. "Fixing" this may cause more situations where a vehicle or passenger is "stuck".
3. Certain routing is a bit finicky near intersections, causing a slight backtracking. The route planner likely needs some further improvement to guarantee nodes are always "forward" near an intersection.
4. Make vehicle/passenger generation more dynamic around potential supply/demand. This could result in a vehicle/passenger leaving the map if they have to wait to long for a match, or also where more vehicles would appear if passengers are waiting longer, or vice versa.
5. User input could be given to switch between closest and simple (sort of like FIFO) matching.
6. User input could be given of coordinates on which to center a map, and thereby call the OSM API to download the related data and image.

## Dependencies for Running Locally

This project is written with C++17.

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
