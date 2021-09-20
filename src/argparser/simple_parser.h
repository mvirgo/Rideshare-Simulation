/**
 * @file simple_parser.h
 * @brief Parse command line arguments for the simulator.
 *
 * @copyright Copyright (c) 2021, Michael Virgo, released under the MIT License.
 *
 */

#ifndef SIMPLE_PARSER_H_
#define SIMPLE_PARSER_H_

#include <string>
#include <unordered_map>

namespace rideshare {

class SimpleParser {
  public:
    std::unordered_map<std::string, std::string> ParseArgs(int argc, char *argv[]);

  private:
    void MissingArgValue(std::string arg);
    std::string ParseMatchType(std::string input_match);
    void ParseNumericInputs(std::string max_objects, std::string name, int min, int max);
    void PrintHelper();
    std::unordered_map<std::string, std::string> SetDefaults();

    const std::string DEFAULT_MAP = "downtown-kc";
    const std::string DEFAULT_MATCH_TYPE = "closest";
    const std::string DEFAULT_MAX_OBJECTS = "10"; // Vehicles & Passengers
    const std::string DEFAULT_MIN_WAIT = "3"; // Wait for next generation
    const std::string DEFAULT_WAIT_RANGE = "2"; // Range of wait time above min
    const int ABSOLUTE_MAX_OBJECTS = 100; // Don't allow higher
    const int ABSOLUTE_MIN_OBJECTS = 0; // Don't allow lower
    const int ABSOLUTE_MIN_WAIT = 1;
    const int ABSOLUTE_MIN_WAIT_RANGE = 0;
};

}  // namespace rideshare

#endif  // SIMPLE_PARSER_H_
