#ifndef PIPELINECREATOR_H
#define PIPELINECREATOR_H

#include <string>
#include <opencv2/opencv.hpp>
#include <fstream>
#include <sstream>
#include <spdlog/spdlog.h>

#define DEFAULT_PIPELINE "gstreamer_pipeline.txt"
#define DECKLINK_PIPELINE "decklink_pipeline.txt"

/**
 * @class PipelineCreator
 * @brief Handles creation of GStreamer pipelines for video capture.
 */
class PipelineCreator 
{
public:
    /**
     * @brief Load GStreamer pipeline configuration from a file.
     * @return The GStreamer pipeline string.
     */
    static std::string loadDefaultPipeline();

    /**
     * @brief Creates a GStreamer pipeline string for capturing video from a Decklink device.
     * @param cameraNumber The camera number to be inserted into the pipeline configuration.
     * @return A string containing the complete GStreamer pipeline with the camera number.
     */
    static std::string CreateDecklinkPipeline(int cameraNumber);

    /**
     * @brief Finds and opens the appropriate video source based on the input name.
     * @param inputName The name of the input source.
     * @param cap Reference to a cv::VideoCapture object to be initialized.
     * @param cameraNumber The camera number to be used for Decklink capture.
     * @return true if the input source is valid and the cv::VideoCapture object is successfully opened, false otherwise.
     */
    static bool findSourceImage(const std::string& inputName, cv::VideoCapture& cap, int cameraNumber);

};

#endif // PIPELINECREATOR_H
