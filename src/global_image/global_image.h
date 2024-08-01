#pragma once

#pragma once

#include <opencv2/opencv.hpp>
#include <mutex>
#include <condition_variable>

namespace GlobalImage 
{
    extern cv::Mat image; // Declare the global image variable
    extern std::mutex imageMutex; // Declare the mutex for image access
    extern std::condition_variable imageCondition; // Declare the condition variable for image access

    /**
     * @brief Updates the global image with a new image.
     * 
     * @param cap The new image to be set.
     */
    void updateImage(const cv::Mat& cap);

    /**
     * @brief Safely retrieves the current global image.
     * 
     * This function ensures that the image is not being updated while it is being retrieved.
     * 
     * @return A clone of the current global image.
     */
    cv::Mat getImage();
}
