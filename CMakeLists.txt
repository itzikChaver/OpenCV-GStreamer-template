# Minimum version of CMake required
cmake_minimum_required(VERSION 3.10)

# Version definition
set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_PATCH 0)
set(VERSION_STRING "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

# Project name
set(PROJECT_N OpenCV_GStreamer_template)
project(${PROJECT_N})

#set CXX
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_options(-fsigned-char)

# Include directories
include_directories(include)

# macro that gets all of the header containing directories. 
MACRO(header_directories return_list includes_base_folder extention )
    FILE(GLOB_RECURSE new_list ${includes_base_folder}/*.${extention})
    SET(dir_list "")
    FOREACH(file_path ${new_list})
        GET_FILENAME_COMPONENT(dir_path ${file_path} PATH)
        SET(dir_list ${dir_list} ${dir_path})
    ENDFOREACH()
    LIST(REMOVE_DUPLICATES dir_list)
    SET(${return_list} ${dir_list})
ENDMACRO()

# Source files
file(GLOB_RECURSE SRCS ${PROJECT_SOURCE_DIR}/src/*.cpp)

# using said macro.
header_directories(INCLUDES ${PROJECT_SOURCE_DIR}/include/ h)

message("src files:")
foreach(file ${SRCS})
        message(STATUS ${file})
endforeach()

message("include directories:")
foreach(dir ${INCLUDES})
        message(STATUS ${dir})
endforeach()

# Generate executable for your main project
add_executable(${PROJECT_N} ${SRCS})

# Define a variable for the installation prefix
set(CMAKE_INSTALL_PREFIX /usr/local)

### Linux specific configuration ###
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 8)
        # Must use std::experimental namespace if older than GCC8
        message("[${PROJECT_NAME}] GCC version less than 8. Using std::experimental namespace.")
        target_compile_definitions(${PROJECT_N} PRIVATE USE_EXPERIMENTAL_FS)
    endif()

    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9)
        # Must explicitly link separate std::filesystem if older than GCC9
        message("[${PROJECT_NAME}] GCC version less than 9. Explicitly linking separate std::filesystem library.")
        target_link_libraries(${PROJECT_N} PRIVATE stdc++fs)
    endif()
endif()

# Check for filesystem library availability
include(CheckCXXSourceCompiles)
set(CMAKE_REQUIRED_FLAGS "-std=c++17")
check_cxx_source_compiles("
    #include <filesystem>
    int main() { std::filesystem::path p; return 0; }
" HAVE_STD_FILESYSTEM)

if(NOT HAVE_STD_FILESYSTEM)
    target_compile_definitions(UsbController PRIVATE -DUSE_EXPERIMENTAL_FILESYSTEM)
    target_link_libraries(UsbController PRIVATE stdc++fs)
endif()

# For OpenCV
find_package(OpenCV REQUIRED)
if(OpenCV_FOUND)
    target_include_directories(${PROJECT_N} PRIVATE ${OpenCV_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_N} PRIVATE ${OpenCV_LIBS})
    message(STATUS "OpenCV found and linked\n")
else()
    message(FATAL_ERROR "OpenCV not found\n")
endif()

# Check for openssl and libssl-dev
message(STATUS "Searching for openssl and libssl-dev...")
find_package(OpenSSL)
if (NOT OpenSSL_FOUND)
    message(FATAL_ERROR "OpenSSL or libssl-dev not found.")
else()
    message(STATUS "OpenSSL and libssl-dev found.")
endif()

# Get OpenSSL version
execute_process(
    COMMAND openssl version
    OUTPUT_VARIABLE openssl_version
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

message(STATUS "OpenSSL version: ${openssl_version}.\n")

# Define OPENSSL_ROOT_DIR if not already defined
if (NOT DEFINED OPENSSL_ROOT_DIR)
    message(STATUS "Define OPENSSL_ROOT_DIR...\n")
    set(OPENSSL_ROOT_DIR "/usr/local") 
endif()

# Set OpenSSL include and lib directories
set(OPENSSL_INCLUDE_DIR "/usr/local/include")
set(OPENSSL_LIBRARIES "/usr/local/lib")

# Add OpenSSL include and lib directories to CMake path
include_directories(${OPENSSL_INCLUDE_DIR})
link_directories(${OPENSSL_LIBRARIES})

# Additional debug info
message(STATUS "OpenSSL include dir: ${OPENSSL_INCLUDE_DIR}")
message(STATUS "OpenSSL libraries dir: ${OPENSSL_LIBRARIES}")
message(STATUS "OpenSSL root dir: ${OPENSSL_ROOT_DIR}")

# Check if OpenSSL targets are found
if(TARGET OpenSSL::Crypto)
    message(STATUS "OpenSSL Crypto target found.")
else()
    message(STATUS "OpenSSL Crypto target not found.")
endif()

if(TARGET OpenSSL::SSL)
    message(STATUS "OpenSSL SSL target found.")
else()
    message(STATUS "OpenSSL SSL target not found.")
endif()

# For httplib
message(STATUS "Searching for httplib...")
find_package(OpenSSL REQUIRED COMPONENTS Crypto SSL)
if (NOT OpenSSL_FOUND)
    message(FATAL_ERROR "OpenSSL components Crypto and SSL not found.\n")
else()
    message(STATUS "OpenSSL components Crypto and SSL found.\n")
endif()

find_package(httplib)
if (NOT httplib_FOUND)
    message(FATAL_ERROR "httplib not found.\n")
else()
    message(STATUS "httplib found.\n")
    target_link_libraries(${PROJECT_N} ${httplib_LIBS})
endif()

# For SPDLOG_LIBRARY
find_package(spdlog REQUIRED)
if(spdlog_FOUND)
  target_link_libraries(${PROJECT_N} ${SPDLOG_LIBRARY})
  message(STATUS "spdlog found!")
else()
  message(STATUS "spdlog not found, attempting to install...")
  set(EXTERNAL_PROJECT_NAME spdlog)
  set(EXTERNAL_PROJECT_URL https://github.com/gabime/spdlog.git)
  # Define the project and its properties
  ExternalProject_Add(
      spdlog
      PREFIX ${DEPS_DIR}/${EXTERNAL_PROJECT_NAME}
      GIT_REPOSITORY ${EXTERNAL_PROJECT_URL}
      GIT_TAG v1.13.0 # Specify the desired release tag
      BUILD_COMMAND ${CMAKE_COMMAND} --build . --config Release -- -j12  # This will build with parallel jobs
      INSTALL_DIR ${CMAKE_INSTALL_PREFIX}  # Install to default CMake install path
  )
endif()

# For fmt
message(STATUS "\nSearching for fmt...")
find_package(fmt REQUIRED)
if(fmt_FOUND)
  target_link_libraries(${PROJECT_N} PUBLIC fmt::fmt)  # Link fmt publicly
  message(STATUS "fmt found!")
else()
  message(STATUS "fmtlib not found, attempting to install...")

  # Run apt-get install command to install fmtlib
  execute_process(
      COMMAND sudo apt-get install -y libfmt-dev
      RESULT_VARIABLE result
  )

  # Check the result of the apt-get install command
  if(result)
      message(FATAL_ERROR "Failed to install fmtlib. Re run with sudo or install it manually and re-run CMake.")
  endif()

  # Re-check for fmtlib after installation
  find_package(fmt REQUIRED)
  if(NOT fmt_FOUND)
      message(FATAL_ERROR "fmtlib is required but could not be found.")
  endif()
  # Add error handling here if fmt is required
endif()

# For Libevent
message(STATUS "\nSearching for Libevent...")
find_package(Libevent REQUIRED)
if(Libevent_FOUND)
    target_link_libraries(${PROJECT_N} PUBLIC ${LIBEVENT_LIBRARIES})
    message(STATUS "Libevent found!")
else()
    message(STATUS "Libevent not found!")
    message(STATUS "Installing Libevent...")
    message(STATUS "Looking for OpenSSL...")
    # Check if libssl-dev is installed
    find_package(OpenSSL REQUIRED)
    if(NOT OpenSSL_FOUND)
        # If libssl-dev is not found, try to install it
        message(STATUS "libssl-dev not found, attempting to install...")

        # Run apt-get install command to install libssl-dev
        execute_process(
            COMMAND sudo apt-get install -y libssl-dev
            RESULT_VARIABLE result
        )

        # Check the result of the apt-get install command
        if(result)
            message(FATAL_ERROR "Failed to install libssl-dev. re-run with sudo or install it manually and re-run CMake.")
        endif()
        # Re-check for libssl-dev after installation
        find_package(OpenSSL REQUIRED)
        if(NOT OpenSSL_FOUND)
            message(FATAL_ERROR "libssl-dev is required but could not be found.")
        endif()
        set(EXTERNAL_PROJECT_NAME libEvent)
        set(EXTERNAL_PROJECT_URL https://github.com/libevent/libevent.git)
        # Define the project and its properties
        ExternalProject_Add(
            LibEvent
            PREFIX ${DEPS_DIR}/${EXTERNAL_PROJECT_NAME}
            GIT_REPOSITORY ${EXTERNAL_PROJECT_URL}
            GIT_TAG release-2.1.12-stable  # Specify the desired release tag
            BUILD_COMMAND ${CMAKE_COMMAND} --build . --config Release -- -j12  # This will build with parallel jobs
            INSTALL_DIR ${CMAKE_INSTALL_PREFIX}  # Install to default CMake install path
        )
    endif()
    # Add error handling here if Libevent is required
endif()

add_compile_options(-fsigned-char)

target_link_libraries(${PROJECT_N} PUBLIC stdc++fs)
target_link_libraries(${PROJECT_N} PUBLIC -lpthread)

list(APPEND src_files 
  ${CMAKE_CURRENT_SOURCE_DIR}/config/gstreamer_pipeline.txt  
  ${CMAKE_CURRENT_SOURCE_DIR}/config/decklink_pipeline.txt
)
                      
# Copy CrSDK build artifacts after CrSDK's build step
add_custom_command(TARGET ${PROJECT_N} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
    ${src_files}  # Source: List of files
    $<TARGET_FILE_DIR:${PROJECT_N}>  # Destination: Main project's build directory 
)

