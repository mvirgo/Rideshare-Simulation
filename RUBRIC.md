# Udacity Project Rubric Points

The C++ Nanodegree Capstone Project requests detail on the specific rubric points addressed by the project (at least five were needed), which I detail below.

## Loops, Functions, I/O

> The project demonstrates an understanding of C++ functions and control structures.

Functions and control structures are present throughout the code. For example:

- `while` loops are used within concurrent classes (like `passenger_queue.cpp` lines 65-97) to check whether to generate a new waiting passenger and read new messages from the ride matcher
- `for` loops are used to iterate through map objects, such as `vehicle_manager.cpp` line 106
- `if` statements are used by `vehicle_manager.cpp` (lines 119-143) to take different actions based on vehicle state
- `switch` statements are used to take actions based on message codes, such as `ride_matcher.cpp` lines 247-271

> The project reads data from a file and process the data, or the program writes data to a file.

In `main.cpp`, the OpenStreetMap data file is read and processed.

## Object Oriented Programming

> The project uses Object Oriented Programming techniques.

Files throughout the project make use of classes and class methods.

> Classes use appropriate access specifiers for class members.

Classes throughout the project use access specifiers (see `object_holder.h` or `passenger_queue.h`, for example).

> Classes abstract implementation details from their interfaces.

Classes throughout the project abstract implementation into `.cpp` files from their interfaces in `.h` files, such as `vehicle_manager.h` and `vehicle_manager.cpp`.

> Classes follow an appropriate inheritance hierarchy.

Data and functions that can be used by more than one class have been moved into parent classes. For example, `ride_matcher.h` shows that the related class inherits from `ConcurrentObject` and `MessageHandler`, as it needs concurrency functionality like the passenger queue and vehicle manager, and simple message handling like the passenger queue. However, it does not inherit from `ObjectHolder`, since it does not actually store the vehicles or passengers itself.

> Derived class functions override virtual base class functions.

This is most apparent with the overriding of `Simulate` in classes inheriting from `ConcurrentObject`, such as within `ride_matcher.h` (line 45) and `ride_matcher.cpp` (lines 94-97).

## Memory Management

> The project uses smart pointers instead of raw pointers.

Shared pointers are used in multiple places throughout the project where needed - `passenger_queue.h`, for example, stores the waiting passengers as shared pointers to the related `Passenger` on line 81.

Note that certain code re-used from the route planning project (primarily `Model` or `RouteModel`) was not further adapted to move away from raw pointers.

## Concurrency

> The project uses multithreading.

While the main thread is used to draw the map itself in `graphics.cpp`, each of the classes inheriting from `ConcurrentObject` make use of separate threads in their `Simulate` functions. For example, see lines 90-93 of `vehicle_manager.cpp`.

> A mutex or lock is used in the project.

There is a shared mutex to protect cout (similar to the concurrent traffic project) in everything that inherits from `ConcurrentObject`, as well as individual mutexes used to protect data structures storing either simple messages (used by ride matcher and passenger queue) or more complex messages (used by the vehicle manager). For example, see the mutex `message_handler.h` line 31, that is used in `passenger_queue.cpp` lines 101 & 108.