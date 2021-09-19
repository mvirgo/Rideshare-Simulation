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
    void PrintHelper();
    std::unordered_map<std::string, std::string> SetDefaults();

    const std::string DEFAULT_MAP = "downtown-kc";
    const std::string DEFAULT_MATCH_TYPE = "closest";
};

}  // namespace rideshare

#endif  // SIMPLE_PARSER_H_
