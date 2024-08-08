#include "video_processor.h"

void VideoProcessor::processVideo(cv::VideoCapture &videoCapture, cv::VideoWriter &writer, std::atomic<bool> &stopProgram) 
{
    cv::Mat frame;
    while (!stopProgram.load()) 
    {
        // Measure time before reading frame
        auto start_time = std::chrono::high_resolution_clock::now();

        if (!videoCapture.read(frame)) 
        {
            if (videoCapture.get(cv::CAP_PROP_POS_FRAMES) >= videoCapture.get(cv::CAP_PROP_FRAME_COUNT)) 
            {
                spdlog::info("Video playback completed.");
            } 
            else 
            {
                spdlog::error("Unable to read frame from video capture");
            }
            break;
        }

        // Print image details
        // printImageDetails(frame);

        // Measure time after reading frame
        auto read_time = std::chrono::high_resolution_clock::now();

        GlobalImage::updateImage(frame);

        if(stopProgram.load())
        {
            break;
        } 

        // Process and display image
        processAndDisplayImage(frame, writer);

        // Measure time after processing frame
        auto process_time = std::chrono::high_resolution_clock::now();

        if(stopProgram.load())
        {
            break;
        } 

        // Calculate elapsed times
        auto read_duration = std::chrono::duration_cast<std::chrono::milliseconds>(read_time - start_time);
        auto process_duration = std::chrono::duration_cast<std::chrono::milliseconds>(process_time - read_time);
        auto total_duration = std::chrono::duration_cast<std::chrono::milliseconds>(process_time - start_time);

        // Print timings
        spdlog::info("Read time: {} ms, Process time: {} ms, Total time: {} ms", read_duration.count(), process_duration.count(), total_duration.count());

        if (cv::waitKey(30) >= 0) 
        {
            break;
        }
    }
}

void VideoProcessor::processAndDisplayImage(cv::Mat &image, cv::VideoWriter &writer) 
{
    if (writer.isOpened()) 
    {
        writer.write(image);
    }
    cv::namedWindow("test", 0);
    cv::imshow("test", image);
}

void VideoProcessor::printImageDetails(const cv::Mat& image) 
{
    if (image.empty()) 
    {
        std::cerr << "Image is empty. Cannot display details." << std::endl;
        return;
    }

    // Resolution
    int width = image.cols;
    int height = image.rows;
    std::cout << "Resolution: " << width << " x " << height << std::endl;

    // Number of channels
    int channels = image.channels();
    std::cout << "Number of Channels: " << channels << std::endl;

    // Depth of the image (in bits per channel)
    int depth = image.depth();
    std::string depthStr;
    switch(depth) 
    {
        case CV_8U:  depthStr = "8-bit unsigned integer"; break;
        case CV_8S:  depthStr = "8-bit signed integer"; break;
        case CV_16U: depthStr = "16-bit unsigned integer"; break;
        case CV_16S: depthStr = "16-bit signed integer"; break;
        case CV_32S: depthStr = "32-bit signed integer"; break;
        case CV_32F: depthStr = "32-bit floating point"; break;
        case CV_64F: depthStr = "64-bit floating point"; break;
        default:     depthStr = "Unknown"; break;
    }
    std::cout << "Depth: " << depthStr << std::endl;

    // Pixel size in bytes
    size_t pixelSize = image.elemSize();
    std::cout << "Pixel Size: " << pixelSize << " bytes" << std::endl;

    // Image format
    std::string format;
    if (channels == 1) 
    {
        format = "Grayscale";
    } 
    else if (channels == 3) 
    {
        format = "BGR (standard for OpenCV)";
    } 
    else if (channels == 4) 
    {
        format = "BGRA (with alpha channel)";
    } 
    else 
    {
        format = "Unknown";
    }
    std::cout << "Format: " << format << std::endl;

    // Image size in bytes
    size_t imageSize = image.total() * image.elemSize();
    std::cout << "Image Size: " << imageSize << " bytes" << std::endl;

    std::cout << "--------------------------------------" << std::endl;
}

