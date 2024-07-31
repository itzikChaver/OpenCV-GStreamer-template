#ifndef VIDEO_CAPTURE_H
#define VIDEO_CAPTURE_H

#include <opencv2/opencv.hpp>
#include <string>
#include <iostream>

class VideoCapture {
public:

    /**
     * @brief Constructor for VideoCapture class.
     * 
     * @param pipeline GStreamer pipeline string.
     */
    VideoCapture(const std::string& pipeline);

    /**
     * @brief Destructor for VideoCapture class.
     */
    ~VideoCapture();
    
    /**
     * @brief Open the video capture pipeline.
     * 
     * @return true if the pipeline was successfully opened, false otherwise.
     */
    bool open();
    
    /**
     * @brief Close the video capture pipeline.
     */
    void close();

    /**
     * @brief Read a frame from the video capture pipeline.
     * 
     * @param frame The output frame.
     * @return true if the frame was successfully read, false otherwise.
     */
    bool read(cv::Mat& frame);

private:
    std::string pipeline_;
    cv::VideoCapture cap_;
};

#endif // VIDEO_CAPTURE_H
