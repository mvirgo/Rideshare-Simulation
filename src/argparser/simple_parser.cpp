/**
 * @file simple_parser.cpp
 * @brief Parse arguments from the command line, or use defaults.
 *
 * @copyright Copyright (c) 2021, Michael Virgo, released under the MIT License.
 *
 */

#include "simple_parser.h"

#include <iostream>
#include <string>
#include <unordered_map>

namespace rideshare {

std::unordered_map<std::string, std::string> SimpleParser::ParseArgs(int argc, char *argv[]) {

    // Set to defaults first
    auto settings = SetDefaults();

    // Loop through and parse all valid args
    for (int i = 0; i < argc; ++i) {
        if (argv[i] == std::string("-h")) {
            PrintHelper();
        } else if (argv[i][0] == '-' && (i+1 >= argc)) {
            MissingArgValue(argv[i]);
        } else if (argv[i] == std::string("-m")) {
            settings["map"] = argv[i+1];
        } else if (argv[i] == std::string("-p")) {
            ParseNumericInputs(argv[i+1], "Passengers", ABSOLUTE_MIN_OBJECTS, ABSOLUTE_MAX_OBJECTS);
            settings["passengers"] = argv[i+1];
        } else if (argv[i] == std::string("-r")) {
            ParseNumericInputs(argv[i+1], "Wait Range", ABSOLUTE_MIN_WAIT_RANGE, ABSOLUTE_MAX_OBJECTS);
            settings["wait_range"] = argv[i+1];
        } else if (argv[i] == std::string("-t")) {
            settings["match"] = ParseMatchType(argv[i+1]);
        } else if (argv[i] == std::string("-v")) {
            ParseNumericInputs(argv[i+1], "Vehicles", ABSOLUTE_MIN_OBJECTS, ABSOLUTE_MAX_OBJECTS);
            settings["vehicles"] = argv[i+1];
        } else if (argv[i] == std::string("-w")) {
            ParseNumericInputs(argv[i+1], "Wait", ABSOLUTE_MIN_WAIT, ABSOLUTE_MAX_OBJECTS);
            settings["wait"] = argv[i+1];
        }
    }

    return settings;
}

void SimpleParser::MissingArgValue(std::string arg) {
    std::cout << "Missing value for given arg: " << arg << ", exiting." << std::endl;
    PrintHelper();
}

std::string SimpleParser::ParseMatchType(std::string input_match) {
    // Make lowercase
    for (auto& ch : input_match) {
        ch = tolower(ch);
    }
    // Make sure it is a valid type
    if (input_match != "closest" && input_match != "simple") {
        std::cout << "Invalid match type given." << std::endl;
        PrintHelper();
    }
    return input_match;
}

void SimpleParser::ParseNumericInputs(std::string max_objects, std::string name, int min, int max) {
    // Check that it is a number
    try {
        int max_input = std::stoi(max_objects);
        // Check that not negative or too high
        if (max_input < min || max_input > max) {
            std::cout << name << " input below min or above max, exiting." << std::endl;
            PrintHelper();
        }
    } catch (...) {
        std::cout << "Non-number given for " << name << " input, exiting." << std::endl;
        PrintHelper();
    }
}

void SimpleParser::PrintHelper() {
    std::cout << "Rideshare Simulation - Valid Arguments" << std::endl;
    std::cout << "-h : Display this helper text. Program will exit." << std::endl;
    std::cout << "-m : Map data file and image name, in /data dir.  Default: "
      << DEFAULT_MAP << std::endl;
    std::cout << "-p : Max passengers in queue.  Min: 0  Max: "
      << ABSOLUTE_MAX_OBJECTS << "  Default: " << DEFAULT_MAX_OBJECTS << std::endl;
    std::cout << "-r : Range, on top of min, to wait to generate passenger.  Min: "
      << ABSOLUTE_MIN_WAIT_RANGE << "  Default: " << DEFAULT_WAIT_RANGE << std::endl;
    std::cout << "-t : Match type, either 'closest' or 'simple'.  Default: "
      << DEFAULT_MATCH_TYPE << std::endl;
    std::cout << "-v : Max vehicles driving.  Min: 0  Max: "
      << ABSOLUTE_MAX_OBJECTS << "  Default: " << DEFAULT_MAX_OBJECTS << std::endl;
    std::cout << "-w : Minimum wait time to generate next waiting passenger.  Min: "
      << ABSOLUTE_MIN_WAIT << "  Default: " << DEFAULT_MIN_WAIT << std::endl;
    // Do not continue the program
    exit(0);
}

std::unordered_map<std::string, std::string> SimpleParser::SetDefaults() {
    std::unordered_map<std::string, std::string> settings;

    // Place all default values
    settings.emplace("map", DEFAULT_MAP);
    settings.emplace("match", DEFAULT_MATCH_TYPE);
    settings.emplace("passengers", DEFAULT_MAX_OBJECTS);
    settings.emplace("vehicles", DEFAULT_MAX_OBJECTS);
    settings.emplace("wait", DEFAULT_MIN_WAIT);
    settings.emplace("wait_range", DEFAULT_WAIT_RANGE);

    return settings;
}

}  // namespace rideshare
