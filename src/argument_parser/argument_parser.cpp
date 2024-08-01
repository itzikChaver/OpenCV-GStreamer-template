#include "argument_parser.h"

bool ArgumentParser::isValidNumber(const std::string &str) 
{
    for (char c : str) 
    {
        if (!std::isdigit(c)) 
        {
            return false;
        }
    }
    return true;
}

void ArgumentParser::parseArguments(int argc, char *argv[], std::string &inputName, int &cameraNumber) 
{
    if (argc < 2) 
    {
        spdlog::info("No video source is selected, the program will use the default pipeline");
        return;
    }

    if (argc > 3) 
    {
        throw std::invalid_argument("Usage: " + std::string(argv[0]) + " <input_name> <camera_number>");
    } 
    else if (argc == 2) 
    {
        inputName = argv[1];

        // Check if the filename ends with ".mp4" or ".avi" or ".webm"
        if (inputName.size() >= 4 && (inputName.compare(inputName.size() - 4, 4, ".mp4") == 0 || inputName.compare(inputName.size() - 4, 4, ".avi") == 0 || inputName.compare(inputName.size() - 4, 4, ".webm") == 0)) 
        {
            spdlog::info("The video source is a video file.");
        } 
        // Check if the filename ends with ".jpg" or ".png" or ".bmp"
        else if (inputName.size() >= 4 && (inputName.compare(inputName.size() - 4, 4, ".jpg") == 0 || inputName.compare(inputName.size() - 4, 4, ".png") == 0 || inputName.compare(inputName.size() - 4, 4, ".bmp") == 0)) 
        {
            spdlog::info("The video source is an image file.");
        } 
        else 
        {
            spdlog::info("The video source is not a video file or image file.");
            throw std::invalid_argument("Usage: " + std::string(argv[0]) + " <video file or image file> ");
        }
    } 
    else 
    {
        inputName = argv[1];
        std::string cameraNumberStr = argv[2];

        if (!isValidNumber(cameraNumberStr)) 
        {
            throw std::invalid_argument("Camera number must be a valid integer.");
        }

        cameraNumber = std::atoi(cameraNumberStr.c_str());

        if (cameraNumber < 0 || cameraNumber > 4) 
        {
            throw std::out_of_range("Camera number must be between 0 and 4.");
        }

        if (inputName == "") 
        {
            throw std::invalid_argument("The video input not exist: " + inputName);
        }
    }
}
