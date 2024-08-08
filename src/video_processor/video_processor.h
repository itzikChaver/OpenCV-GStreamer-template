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

    /**
     * @brief Processes and displays a single image frame.
     * @param image Reference to the image frame to be processed.
     * @param writer Reference to a cv::VideoWriter object.
     */
    static void processAndDisplayImage(cv::Mat &image, cv::VideoWriter &writer);

    /**
     * @brief Prints detailed information about an image, including its resolution, format, pixel size, and memory size.
     *
     * This function extracts and displays metadata about a given image matrix (`cv::Mat`), such as:
     * - Number of channels: Indicates whether the image is grayscale (1 channel) or colored (3 channels).
     * - Depth: Describes the bit-depth of each channel in the image, indicating the precision of pixel values.
     * - Pixel size: The amount of memory used to store a single pixel, calculated based on the number of channels and the depth.
     * - Format: The color format used by the image, typically BGR for images in OpenCV.
     * - Image size: The total memory size occupied by the image, calculated by multiplying the pixel size by the total number of pixels.
     *
     * @param image The image matrix (cv::Mat) to analyze. The image should be a valid OpenCV matrix.
     * 
     * @note The function supports common image formats used in OpenCV, particularly BGR format for color images.
     * 
     * @throws std::invalid_argument if the provided image matrix is empty or invalid.
     *
     * @example
     * cv::Mat img = cv::imread("image.jpg");
     * printImageDetails(img);
     * 
     * @output
     * Number of Channels: 3
     * Depth: 8-bit unsigned integer
     * Pixel Size: 3 bytes
     * Format: BGR (standard for OpenCV)
     * Image Size: 24698880 bytes
     */
    static void printImageDetails(const cv::Mat& image);
};

#endif // VIDEOPROCESSOR_H
