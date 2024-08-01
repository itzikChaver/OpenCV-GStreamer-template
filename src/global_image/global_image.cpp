#include "global_image.h"

namespace GlobalImage 
{
    cv::Mat image;                          // Definition of the global image variable
    std::mutex imageMutex;                  // Definition of the mutex for image access
    std::condition_variable imageCondition; // Definition of the condition variable for image access

    /**
     * @brief Updates the global image with a new image.
     * 
     * @param cap The new image to be set.
     */
    void updateImage(const cv::Mat& cap) 
    {
        std::unique_lock<std::mutex> lock(imageMutex); // Lock the mutex before accessing the image
        image = cap.clone(); // Update image from the video capture
        imageCondition.notify_all(); // Notify all waiting threads that the image has been updated
    }

    /**
     * @brief Safely retrieves the current global image.
     * 
     * This function ensures that the image is not being updated while it is being retrieved.
     * 
     * @return A clone of the current global image.
     */
    cv::Mat getImage() 
    {
        std::unique_lock<std::mutex> lock(imageMutex); // Lock the mutex before accessing the image
        return image.clone(); // Return a clone of the current image
    }
}
