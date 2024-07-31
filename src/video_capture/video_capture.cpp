#include "video_capture.h"


VideoCapture::VideoCapture(const std::string& pipeline)
    : pipeline_(pipeline), cap_(cv::VideoCapture(pipeline, cv::CAP_GSTREAMER)) {}

VideoCapture::~VideoCapture() 
{
    close();
}

bool VideoCapture::open() 
{
    if (!cap_.isOpened()) 
    {
        std::cerr << "Error: Unable to open video capture with pipeline: " << pipeline_ << std::endl;
        return false;
    }
    return true;
}

void VideoCapture::close() 
{
    if (cap_.isOpened()) 
    {
        cap_.release();
    }
}

bool VideoCapture::read(cv::Mat& frame) 
{
    return cap_.read(frame);
}
