#ifndef VIDEOPROCESSOR_H
#define VIDEOPROCESSOR_H

#include <opencv2/opencv.hpp>
#include <atomic>
#include <spdlog/spdlog.h>
#include <chrono>

#include "../global_image/global_image.h"

/**
 * @class VideoProcessor
 * @brief Handles processing and displaying video frames.
 */
class VideoProcessor {
public:
    /**
     * @brief Processes video frames from a cv::VideoCapture object.
     * @param videoCapture Reference to a cv::VideoCapture object.
     * @param writer Reference to a cv::VideoWriter object.
     * @param stopProgram Reference to an atomic boolean flag to stop the video processing loop.
     */
    static void processVideo(cv::VideoCapture &videoCapture, cv::VideoWriter &writer, std::atomic<bool> &stopProgram);

private:
    /**
     * @brief Processes and displays a single image frame.
     * @param image Reference to the image frame to be processed.
     * @param writer Reference to a cv::VideoWriter object.
     */
    static void processAndDisplayImage(cv::Mat &image, cv::VideoWriter &writer);
};

#endif // VIDEOPROCESSOR_H
