#include "video_capture/video_capture.h"
#include <iostream>
#include <fstream>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/core/utils/logger.hpp>
#include <spdlog/spdlog.h>
#include <fmt/format.h>

#define DEFAULT_PIPELINE "gstreamer_pipeline.txt"
#define DECKLINK_PIPELINE "decklink_pipeline.txt"


bool isValidNumber(const std::string &str);
void parseArguments(int argc, char *argv[], std::string &inputName, int &cameraNumber);
std::string CreateDecklinkPipeline(int cameraNumber);
bool findSourceImage(const std::string& inputName, cv::VideoCapture& cap, int cameraNumber);
std::string loadDefaultPipeline();
void processAndDisplayImage(cv::Mat &image, cv::VideoWriter &writer);

int main(int argc, char *argv[]) 
{
    try
    {
        std::string inputName;
        int cameraNumber;

        // Validate and parse command-line arguments.
        parseArguments(argc, argv, inputName, cameraNumber);

        cv::VideoWriter writer;
        cv::VideoCapture videoCapture;
        cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_ERROR);

        // Check if the input source is a video file or a camera.
        bool isFindSourceImage = findSourceImage(inputName, videoCapture, cameraNumber);

        if (!isFindSourceImage)
        {
            throw std::runtime_error("Failed to find source image");
        }
        
        if (!videoCapture.isOpened()) 
        {
            return EXIT_FAILURE;
        }

        cv::Mat frame;
        while (true) 
        {
            if (!videoCapture.read(frame)) 
            {
                spdlog::error("Unable to read frame from video capture");
                break;
            }

            processAndDisplayImage(frame, writer);

            if (cv::waitKey(30) >= 0) 
            {
                break;
            }
        }

        videoCapture.~VideoCapture();
        spdlog::info("Resources cleaned up.");
        std::cout << "EXIT " << std::endl;
        return EXIT_SUCCESS;
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << "runtime error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch (const std::invalid_argument &e)
    {
        std::cerr << "Invalid argument: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch (const std::out_of_range &e)
    {
        std::cerr << "Out of range error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch (const std::exception &e)
    {
        std::cout << "Resources cleaned up." << std::endl;
        return EXIT_FAILURE; 
    }
}

/**
 * @brief Checks if a given string represents a valid positive integer.
 *
 * This function iterates over each character in the input string and checks if all characters
 * are digits (0-9). If all characters are digits, the function returns true, indicating that
 * the string is a valid positive integer. If any character is not a digit, the function returns
 * false.
 *
 * @param str The input string to be checked.
 * @return true if the string represents a valid positive integer, false otherwise.
 */
bool isValidNumber(const std::string &str)
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

/**
 * @brief Parses and validates command-line arguments.
 *
 * This function validates the command-line arguments provided to the program. It checks if the
 * required number of arguments is provided, if the camera number is a valid integer within the
 * specified range (0 to 4), and if the input file name is not empty. If any of the checks fail,
 * the function throws an appropriate exception with a descriptive error message.
 *
 * @param argc The number of command-line arguments.
 * @param argv The array of command-line argument strings.
 * @param gCameraNumber Reference to an integer where the parsed camera number will be stored.
 * @param input_name Reference to a string where the parsed input file name will be stored.
 * @param isCamera Reference to a boolean where true will be stored if the software is using a camera or video/image.
 * 
 * @throws std::invalid_argument if the number of arguments is insufficient, the camera number is
 * not a valid integer, or the input file name is not provided.
 * @throws std::out_of_range if the camera number is not within the valid range (0 to 4).
 */
void parseArguments(int argc, char *argv[], std::string &inputName, int &cameraNumber)
{
    if(argc < 2)
    {
        spdlog::info("No video source is selected, the program will use the default pipeline");
        return;
    }

    if(argc > 3)
    {
        throw std::invalid_argument("Usage: " + std::string(argv[0]) + " <input_name> <camera_number>");
    }
    else if (argc == 2)
    {
        inputName = argv[1];

        // Check if the filename ends with ".mp4" or ".avi" or ".webm"
        if (inputName.size() >= 4 && (inputName.compare(inputName.size() - 4, 4, ".mp4") == 0 || inputName.compare(inputName.size() - 4, 4, ".avi") == 0 || inputName.compare(inputName.size() - 4, 4, ".webm") == 0)) 
        {
            spdlog::info("The video source is an video file.");
        } 
        // Check if the filename ends with ".jpg" or ".png" or ".bmp"
        else if (inputName.size() >= 4 && (inputName.compare(inputName.size() - 4, 4, ".jpg") == 0 || inputName.compare(inputName.size() - 4, 4, ".png") == 0 || inputName.compare(inputName.size() - 4, 4, ".bmp") == 0))
        {
            spdlog::info("The video source is an image file.");
        }
        else 
        {
            spdlog::info("The video source is not an video file or image file.");
            throw std::invalid_argument("Usage: " + std::string(argv[0]) + " <video file(.mp4)> ");
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

/**
 * @brief Creates a GStreamer pipeline string for capturing video from a Decklink device.
 * 
 * This function reads a Decklink pipeline configuration template from a file and replaces
 * a placeholder with the specified camera number. The template file should contain the
 * placeholder "$CAMERA_NUMBER$" where the camera number should be inserted.
 * 
 * @param cameraNumber The camera number to be inserted into the pipeline configuration.
 * @return A string containing the complete GStreamer pipeline with the camera number.
 *         If the configuration file cannot be opened or the placeholder is not found,
 *         an empty string is returned and an error is logged.
 */
std::string CreateDecklinkPipeline(int cameraNumber) 
{
    // Read decklink pipeline configuration from file
    std::ifstream config_file(std::string(DECKLINK_PIPELINE));
    if (!config_file.is_open()) {
        spdlog::error(" Could not open pipeline configuration file.");
        return "";
    }

    std::stringstream buffer;
    buffer << config_file.rdbuf();
    std::string pipeline_template = buffer.str();

    // Update camera number in the pipeline template
    size_t pos = pipeline_template.find("$CAMERA_NUMBER$");
    if (pos != std::string::npos) 
    {
        pipeline_template.replace(pos, std::string("$CAMERA_NUMBER$").length(), std::to_string(cameraNumber));
    }
    else 
    {
        spdlog::error("Camera number placeholder not found in pipeline template.");
        return "";
    }

    return pipeline_template;
}

/**
 * @brief Finds and opens the appropriate video source based on the input name.
 * 
 * This function determines the type of input source (video file, image file, or Decklink)
 * and initializes a cv::VideoCapture object accordingly. It also supports loading a default
 * pipeline if the input name is empty.
 * 
 * @param inputName The name of the input source. This can be a file path (for video or image files),
 *                  "decklink" for capturing from a Decklink device, or an empty string for the default pipeline.
 * @param cap Reference to a cv::VideoCapture object to be initialized.
 * @param cameraNumber The camera number to be used for Decklink capture.
 * @return true if the input source is valid and the cv::VideoCapture object is successfully opened, false otherwise.
 */
bool findSourceImage(const std::string& inputName, cv::VideoCapture& cap, int cameraNumber)
{
    if (inputName.find(".mp4") != std::string::npos || inputName.find(".avi") != std::string::npos || inputName.find(".webm") != std::string::npos) 
    {
        cap = cv::VideoCapture("filesrc location="+inputName+" ! qtdemux ! h264parse ! nvv4l2decoder enable-max-performance=0 ! nvvidconv ! video/x-raw, format=BGRx ! videoconvert ! video/x-raw,format=BGR ! appsink max-buffers=1 drop=True", cv::CAP_GSTREAMER);
        if (!cap.isOpened()) 
        {
            spdlog::error("Invalid input source: {}", inputName.c_str());
            return false;
        }
    } 
    else if (inputName.find(".jpg") != std::string::npos || inputName.find(".png") != std::string::npos || inputName.find(".bmp") != std::string::npos) 
    {
        if (cv::imread(inputName).empty()) 
        {
            spdlog::error("Invalid input source: {}", inputName.c_str());
            return false;
        }
    } 
    else 
    {
        if (inputName == "decklink") 
        {
            std::string pipeline = CreateDecklinkPipeline(cameraNumber);
            spdlog::info("Using Decklink pipeline: {}", pipeline);
            cap = cv::VideoCapture(pipeline, cv::CAP_GSTREAMER);
        } 

        if(inputName.empty())
        {
            std::string pipeline = loadDefaultPipeline();
            cap = cv::VideoCapture(pipeline, cv::CAP_GSTREAMER);
        }
    }
    return true;
}

/**
 * @brief Load GStreamer pipeline configuration from a file.
 * 
 * @param filename The name of the configuration file.
 * @return The GStreamer pipeline string.
 */
std::string loadDefaultPipeline() 
{
    std::ifstream file(std::string(DEFAULT_PIPELINE));
    if (!file.is_open()) 
    {
        spdlog::error("Error: Unable to open configuration file: {}", std::string(DEFAULT_PIPELINE));
        exit(EXIT_FAILURE);
    }

    std::string pipeline;
    std::getline(file, pipeline);
    file.close();
    return pipeline;
}

/**
 * @brief Processes and displays the given image.
 *
 * This function writes the image to a video file if the video writer is opened.
 * It then creates a window named "test" and displays the image in that window.
 *
 * @param image The image to be processed and displayed.
 * @param writer A reference to the cv::VideoWriter object used to write the image to a video file.
 */
void processAndDisplayImage(cv::Mat &image, cv::VideoWriter &writer)
{
    if (writer.isOpened())
    {
        writer.write(image);
    }
    cv::namedWindow("test", 0);
    cv::imshow("test", image);
}
