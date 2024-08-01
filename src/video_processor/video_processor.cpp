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
