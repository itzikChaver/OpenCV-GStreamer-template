## OpenCV-GStreamer-template

A comprehensive template project for pulling video from a camera using OpenCV and GStreamer, designed to process and analyze each frame in a multi-threaded environment safely.

## Table of Contents

1. [Prerequisites](#prerequisites)
2. [Build Instructions](#build-instructions)
3. [Run Instructions](#run-instructions)
4. [Configuration](#configuration)
5. [Project Structure](#project-structure)
6. [Usage Example](#usage-example)
7. [License](#license)

## Prerequisites

- OpenCV
- GStreamer
- CMake
- spdlog
- fmt

Ensure you have these libraries installed on your system before proceeding with the build instructions.

## Build Instructions

1. Clone the repository:
    ```sh
    git clone https://github.com/itzikChaver/OpenCV-GStreamer-template.git
    cd OpenCV-GStreamer-template
    ```

2. Create a build directory:
    ```sh
    mkdir build
    cd build
    ```

3. Run CMake to configure the project:
    ```sh
    cmake ..
    ```

4. Build the project:
    ```sh
    make
    ```

## Run Instructions

1. Ensure your camera is connected and the device is correctly specified in `config/decklink_pipeline.txt`.

2. Run the executable:

    ### For running the project in default mode
        ```sh
        ./OpenCV_GStreamer_template
        ```
    - The project will run and use the default pipeline found in the gstreamer_pipeline.txt file, it can be changed to draw from a camera connected to the computer (for example a webcam).

    ### For a capture card (for example Decklink)
        ```sh
        ./OpenCV_GStreamer_template <input_name> [camera_number]
        ```
    - `<input_name>`: The name of the input source (The name of the company that produces the capture card).
    - `[camera_number]`: The camera number to be used for Decklink capture.

    ### For a video file
        ```sh
        ./OpenCV_GStreamer_template <video_file_name> 
        ```
    - `<video_file_name>`: The name of the video file source.

    ### For a image file
        ```sh
        ./OpenCV_GStreamer_template <image_file_name> 
        ```
    - `<image_file_name>`: The name of the image file source.


## Configuration

The GStreamer pipeline configuration for decklink capture card is stored in `config/decklink_pipeline.txt`. 

### Default Pipeline Example

Default GStreamer pipeline. Example:
```
videotestsrc pattern=ball ! videoconvert ! video/x-raw, format=BGR ! appsink
```

## Project Structure

The project is organized into several classes, each handling a specific part of the process:

1. **ArgumentParser**: Handles parsing and validation of command-line arguments.
2. **PipelineCreator**: Manages the creation of GStreamer pipelines for video capture.
3. **VideoProcessor**: Processes video frames, including displaying and analyzing each frame.

### Header and Implementation Files

- `ArgumentParser.h` and `ArgumentParser.cpp`
- `PipelineCreator.h` and `PipelineCreator.cpp`
- `VideoProcessor.h` and `VideoProcessor.cpp`

## Usage Example

Here is an example of how to integrate image analytics in the video processing loop. Assume you have an image analytics function `analyzeImage`:

### Image Analytics Function

```cpp
void analyzeImage(const cv::Mat& image) 
{
    // Perform your image analysis here
    cv::Mat result = image.clone();  // Example of processing
    // ... Your analytics code ...
    cv::imshow("Analytics Result", result);
}
```

### Integration in `VideoProcessor`

In the `processVideo` function of `VideoProcessor`, you can integrate the image analytics function:

```cpp
void VideoProcessor::processVideo(cv::VideoCapture &videoCapture, cv::VideoWriter &writer, std::atomic<bool> &stopProgram) {
    cv::Mat frame;
    while (!stopProgram.load()) 
    {
        auto start_time = std::chrono::high_resolution_clock::now();

        if (!videoCapture.read(frame)) {
            if (videoCapture.get(cv::CAP_PROP_POS_FRAMES) >= videoCapture.get(cv::CAP_PROP_FRAME_COUNT)) {
                spdlog::info("Video playback completed.");
            } else {
                spdlog::error("Unable to read frame from video capture");
            }
            break;
        }

        auto read_time = std::chrono::high_resolution_clock::now();

        // Update global image for multi-threaded access
        GlobalImage::updateImage(frame);

        // Process and display image
        processAndDisplayImage(frame, writer);

        // Analyze image in a separate thread
        std::thread analysisThread([]() {
            cv::Mat image = GlobalImage::getImage();
            analyzeImage(image);
        });
        analysisThread.detach();

        auto process_time = std::chrono::high_resolution_clock::now();

        auto read_duration = std::chrono::duration_cast<std::chrono::milliseconds>(read_time - start_time);
        auto process_duration = std::chrono::duration_cast<std::chrono::milliseconds>(process_time - read_time);
        auto total_duration = std::chrono::duration_cast<std::chrono::milliseconds>(process_time - start_time);

        spdlog::info("Read time: {} ms, Process time: {} ms, Total time: {} ms", read_duration.count(), process_duration.count(), total_duration.count());

        if (cv::waitKey(30) >= 0) {
            break;
        }
    }
}
```

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

