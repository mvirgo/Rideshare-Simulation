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
            ParseMaxObjects(argv[i+1]);
            settings["passengers"] = argv[i+1];
        } else if (argv[i] == std::string("-t")) {
            settings["match"] = ParseMatchType(argv[i+1]);
        } else if (argv[i] == std::string("-v")) {
            ParseMaxObjects(argv[i+1]);
            settings["vehicles"] = argv[i+1];
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

void SimpleParser::ParseMaxObjects(std::string max_objects) {
    // Check that it is a number
    try {
        int max_input = std::stoi(max_objects);
        // Check that not negative or too high
        if (max_input < 0 || max_input > ABSOLUTE_MAX_OBJECTS) {
            std::cout << "Max object input below min or above max, exiting." << std::endl;
            PrintHelper();
        }
    } catch (...) {
        std::cout << "Non-number given for a max object input, exiting." << std::endl;
        PrintHelper();
    }
}

void SimpleParser::PrintHelper() {
    std::cout << "Rideshare Simulation - Valid Arguments" << std::endl;
    std::cout << "-h : Display this helper text. Program will exit." << std::endl;
    std::cout << "-m : Map data file and image name, in /data dir.  Default: downtown-kc" << std::endl;
    std::cout << "-p : Max passengers in queue.  Min: 0  Max: " << ABSOLUTE_MAX_OBJECTS << "  Default: 10" << std::endl;
    std::cout << "-t : Match type, either 'closest' or 'simple'.  Default: closest" << std::endl;
    std::cout << "-v : Max vehicles driving.  Min: 0  Max: " << ABSOLUTE_MAX_OBJECTS << "  Default: 10" << std::endl;
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

    return settings;
}

}  // namespace rideshare
