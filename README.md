# OpenCV-GStreamer-template
A template project for pulling video from a camera using OpenCV and GStreamer

## Prerequisites

- OpenCV
- GStreamer
- CMake

## Build Instructions

1. Create a build directory:
    ```sh
    mkdir build
    cd build
    ```

2. Run CMake to configure the project:
    ```sh
    cmake ..
    ```

3. Build the project:
    ```sh
    make
    ```

## Run Instructions

1. Ensure your camera is connected and the device is correctly specified in `config/gstreamer_pipeline.txt`.

2. Run the executable:
    ```sh
    ./OpenCV_GStreamer_Project
    ```

## Configuration

The GStreamer pipeline configuration is stored in `config/gstreamer_pipeline.txt`. Modify this file to change the pipeline settings.

## License

This project is licensed under the MIT License - see the LICENSE file for details.