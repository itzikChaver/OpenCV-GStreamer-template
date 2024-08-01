#include "pipeline_creator.h"

std::string PipelineCreator::loadDefaultPipeline() 
{
    std::ifstream file(std::string(DEFAULT_PIPELINE));
    if (!file.is_open()) 
    {
        spdlog::error("Error: Unable to open configuration file: {}", std::string(DEFAULT_PIPELINE));
        exit(EXIT_FAILURE);
    }

    std::string pipeline;
    std::getline(file, pipeline);
    file.close();
    return pipeline;
}

std::string PipelineCreator::CreateDecklinkPipeline(int cameraNumber) 
{
    // Read decklink pipeline configuration from file
    std::ifstream config_file(std::string(DECKLINK_PIPELINE));
    if (!config_file.is_open()) 
    {
        spdlog::error(" Could not open pipeline configuration file.");
        return "";
    }

    std::stringstream buffer;
    buffer << config_file.rdbuf();
    std::string pipeline_template = buffer.str();

    // Update camera number in the pipeline template
    size_t pos = pipeline_template.find("$CAMERA_NUMBER$");
    if (pos != std::string::npos) 
    {
        pipeline_template.replace(pos, std::string("$CAMERA_NUMBER$").length(), std::to_string(cameraNumber));
    } 
    else 
    {
        spdlog::error("Camera number placeholder not found in pipeline template.");
        return "";
    }

    return pipeline_template;
}

bool PipelineCreator::findSourceImage(const std::string& inputName, cv::VideoCapture& cap, int cameraNumber) 
{
    if (inputName.find(".mp4") != std::string::npos || inputName.find(".avi") != std::string::npos || inputName.find(".webm") != std::string::npos) 
    {
        cap = cv::VideoCapture("filesrc location="+inputName+" ! qtdemux ! h264parse ! nvv4l2decoder enable-max-performance=0 ! nvvidconv ! video/x-raw, format=BGRx ! videoconvert ! video/x-raw,format=BGR ! appsink max-buffers=1 drop=True", cv::CAP_GSTREAMER);
        if (!cap.isOpened()) 
        {
            spdlog::error("Invalid input source: {}", inputName.c_str());
            return false;
        }
    } 
    else if (inputName.find(".jpg") != std::string::npos || inputName.find(".png") != std::string::npos || inputName.find(".bmp") != std::string::npos) 
    {
        if (cv::imread(inputName).empty()) 
        {
            spdlog::error("Invalid input source: {}", inputName.c_str());
            return false;
        }
    } 
    else 
    {
        if (inputName == "decklink") 
        {
            std::string pipeline = CreateDecklinkPipeline(cameraNumber);
            spdlog::info("Using Decklink pipeline: {}", pipeline);
            cap = cv::VideoCapture(pipeline, cv::CAP_GSTREAMER);
        } 

        if (inputName.empty()) 
        {
            std::string pipeline = loadDefaultPipeline();
            cap = cv::VideoCapture(pipeline, cv::CAP_GSTREAMER);
        }
    }
    return true;
}
