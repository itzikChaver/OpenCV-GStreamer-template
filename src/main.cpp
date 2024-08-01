#include <iostream>
#include <atomic>
#include <opencv2/opencv.hpp>
#include <opencv2/core/utils/logger.hpp>
#include <spdlog/spdlog.h>
#include <chrono>

#include "argument_parser/argument_parser.h"
#include "pipeline_creator/pipeline_creator.h"
#include "video_processor/video_processor.h"

std::atomic<bool> stopProgram(false);

int main(int argc, char *argv[]) 
{
    try
    {
        std::string inputName;
        int cameraNumber;

        // Validate and parse command-line arguments.
        ArgumentParser::parseArguments(argc, argv, inputName, cameraNumber);

        cv::VideoWriter writer;
        cv::VideoCapture videoCapture;
        cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_ERROR);

        // Check if the input source is a video file or a camera.
        bool isFindSourceImage = PipelineCreator::findSourceImage(inputName, videoCapture, cameraNumber);

        if (!isFindSourceImage)
        {
            throw std::runtime_error("Failed to find source image");
        }

        if (!videoCapture.isOpened()) 
        {
            cv::Mat image = cv::imread(inputName);
            if (!image.empty()) 
            {
                cv::namedWindow("image", 0);
                cv::imshow("image", image);
                spdlog::info("Displaying image for 5 seconds...");
                cv::waitKey(5000); // wait for 5 seconds
                return EXIT_SUCCESS;
            } 
            else 
            {
                spdlog::error("Invalid input source: {}", inputName.c_str());
                return EXIT_FAILURE;
            }
        }

        VideoProcessor::processVideo(videoCapture, writer, stopProgram);

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
