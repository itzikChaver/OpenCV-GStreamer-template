#ifndef ARGUMENTPARSER_H
#define ARGUMENTPARSER_H

#include <string>
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <cstdlib>

/**
 * @class ArgumentParser
 * @brief Handles parsing and validation of command-line arguments.
 */
class ArgumentParser 
{
public:
    /**
     * @brief Checks if a given string represents a valid positive integer.
     * @param str The input string to be checked.
     * @return true if the string represents a valid positive integer, false otherwise.
     */
    static bool isValidNumber(const std::string &str);

    /**
     * @brief Parses and validates command-line arguments.
     * @param argc The number of command-line arguments.
     * @param argv The array of command-line argument strings.
     * @param inputName Reference to a string where the parsed input file name will be stored.
     * @param cameraNumber Reference to an integer where the parsed camera number will be stored.
     * @throws std::invalid_argument if the number of arguments is insufficient, the camera number is not a valid integer, or the input file name is not provided.
     * @throws std::out_of_range if the camera number is not within the valid range (0 to 4).
     */
    static void parseArguments(int argc, char *argv[], std::string &inputName, int &cameraNumber);
};

#endif // ARGUMENTPARSER_H
