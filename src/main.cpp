#include "video_capture/video_capture.h"
#include <iostream>
#include <fstream>
#include <string>
#include <opencv2/opencv.hpp>

cv::VideoWriter writer;

std::string loadPipelineConfig(const std::string& filename);
void processAndDisplayImage(cv::Mat &image, cv::VideoWriter &writer);

int main(int argc, char** argv) 
{
    std::string configFilePath = "gstreamer_pipeline.txt";
    std::string pipeline = loadPipelineConfig(configFilePath);

    VideoCapture videoCapture(pipeline);

    if (!videoCapture.open()) 
    {
        return -1;
    }

    cv::Mat frame;
    while (true) 
    {
        if (!videoCapture.read(frame)) 
        {
            std::cerr << "Error: Unable to read frame from video capture" << std::endl;
            break;
        }

        processAndDisplayImage(frame, writer);

        if (cv::waitKey(30) >= 0) 
        {
            break;
        }
    }

    videoCapture.close();
    return 0;
}

/**
 * @brief Load GStreamer pipeline configuration from a file.
 * 
 * @param filename The name of the configuration file.
 * @return The GStreamer pipeline string.
 */
std::string loadPipelineConfig(const std::string& filename) 
{
    std::ifstream file(filename);
    if (!file.is_open()) 
    {
        std::cerr << "Error: Unable to open configuration file: " << filename << std::endl;
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
