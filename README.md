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

## Command Line Arguments

While no arguments are required when running the program, there are a number of things you can change (use `-h` to see all):

- `-m`: Change between map data files. This defaults to the `downtown-kc`, or can be `arc-paris`, or others you add into the `data` dir. This would need to be both the OSM data file and an image to draw onto.
- `-p`: Max number of passengers to go in the queue; the map will start with half of these, and generate more over time up to this value.
- `-r`: Range of time, on top of the minimum wait (see `-w` below), to wait to check if the next passenger can be generated.
- `-t`: Match type, either `closest` (default) or `simple`. Closest match goes to the relatively closest vehicle, or simple matching is like FIFO, where the first passenger request and first open vehicle are matched.
- `-v`: Max number of vehicles driving on the map.
- `-w`: Minimum wait time to generate the next waiting passenger (plus the range from `-r`, although you don't have to give both). e.g. A min wait of 3 seconds, plus a range of 2 seconds, will cause passengers to be generated every 3-5 seconds, if below the max passengers allowed in the queue.

Each of the above has a default value that will be used if the related argument is not given to the program at runtime. Certain arguments also have minimum and maximum values; for example, at the time of writing, passengers and vehicles max out at 100 and cannot be negative. If you really want to change those values further, you'd need to change them in the code (it can work with at least up to 1000 passengers and vehicles, but is sluggish at the start, while 100 keeps things fairly smooth).

## Future Improvement Areas

1. Passengers currently "teleport" to the vehicle when the vehicle reaches the closest node on the road to the passenger location, and also teleport out when similarly reaching the closest road node to the final destination. This can be improved to show the passenger "walk" to and from the vehicle instead.
2. Vehicles currently ignore the directions of streets. "Fixing" this may cause more situations where a vehicle or passenger is "stuck".
3. Certain routing is a bit finicky near intersections, causing a slight backtracking. The route planner likely needs some further improvement to guarantee nodes are always "forward" near an intersection.
4. Make vehicle/passenger generation more dynamic around potential supply/demand. This could result in a vehicle/passenger leaving the map if they have to wait to long for a match, or also where more vehicles would appear if passengers are waiting longer, or vice versa.
5. User input could be given of coordinates on which to center a map, and thereby call the OSM API to download the related data and image.

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

## File / Class Structure

The `src` directory contains the primary code files, along with the `thirdparty/pugixml` directory that helps to read the OpenStreetMap data files. Within the `src` directory, the structure is as follows:

- `main.cpp` - reads map data, then starts simulating everything
- `argparser` - classes handling parsing of command line arguments
  - `simple_parser.*` - parsing of arguments, along with containing the defaults and any relevant min or max values
- `concurrent/` - classes that run concurrently or support such concurrency
  - `concurrent_object.*` - parent class of concurrency (for vehicle manager, passenger queue, and ride matcher). Also holds a shared mutex for its children to use in protecting cout
  - `message_handler.h` - parent class used by children that can make use of `simple_message` for activating different functions concurrently. Helps store messages for reading in the next cycle of a thread
  - `object_holder.h` - parent class of those that will generate and hold map objects (vehicle manager and passenger queue). Sets the max of these to be on the map at any given point
  - `passenger_queue.*`- handles all waiting passengers prior to pickup, such as requesting to be matched
  - `ride_matcher.*` - makes matches between empty vehicles and waiting passengers, and communicates between each during arrival/pickup
  - `simple_message.*` - simple struct for passing simple messages by classes that inherit from `message_handler`. The message code here is based on an enum that should be within the classes that can receive such messages
  - `vehicle_manager.*` - handles generating vehicles, requesting to be matched to a passenger, transitioning them between states (including pick up of passengers), smoothly moving them across their map paths, and removing any stuck vehicles
- `map_object/` - classes that are drawn on the output map (vehicles and passengers)
  - `map_object.h` - parent class used for objects to be drawn and map, including adding random color to distinguish objects. Holds position, destination and path information, as well as failure information (used to potentially remove stuck objects)
  - `passenger.h` - stores information on whether a ride has been requested, and shapes to be drawn on the map
  - `vehicle.*` - handles state transitions (e.g. heading to passenger -> waiting -> driving passenger), pick up and drop off of a passenger, and incrementing along its determined route path, along with shapes to be drawn on the map
- `mapping/` - classes for handling the OSM data and map positions
  - `coordinate.h` - basic struct for storing x, y point and checking equality of two points
  - `model.*` - originally from route planning project; handles reading OSM data and coming up with random map positions for vehicle/passenger generation
  - `route_model.*` - child of `model` and also from route planning project; adds more functionality to help with A* Search, such as storing node information used by the `route_planner`
- `routing/` - classes for planning routes between two points
  - `route_planner.*` - uses A* Search to try to plan route between two points. Called by both vehicles and passengers to make sure their destinations are reachable (otherwise they may be removed from the sim)
- `visual/` - classes that handle visualization of the simulation
  - `graphics.*` - loops through drawing vehicles / passengers at each time step, including adjusting their positions onto the map image

## Rubric Points

The Capstone submission through Udacity required pointing out specific rubric items addressed (at least five) and where they were addressed at in the code, see [RUBRIC.md](RUBRIC.md) for details.