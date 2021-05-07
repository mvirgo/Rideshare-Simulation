cmake_minimum_required(VERSION 3.11.3)

set(CMAKE_CXX_STANDARD 17)
project(Rideshare_Simulator)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread")

find_package(OpenCV 4.1 REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
link_directories(${OpenCV_LIBRARY_DIRS})
add_definitions(${OpenCV_DEFINITIONS})

add_subdirectory(thirdparty/pugixml)

# Find all executables
file(GLOB_RECURSE project_SRCS src/*.cpp src/*.h)

# Add project executable
add_executable(rideshare_simulation ${project_SRCS})

# Include project directories
target_include_directories(rideshare_simulation PUBLIC src)
target_include_directories(rideshare_simulation PUBLIC src/*)

# Link everything together
target_link_libraries(rideshare_simulation pugixml ${OpenCV_LIBRARIES})