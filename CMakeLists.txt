cmake_minimum_required(VERSION 3.14)
project(CLIAudioEditor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release)
endif()

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_library(audiocore
    src/AudioClip.cpp
    src/WavReader.cpp
    src/WavWriter.cpp
    src/Crop.cpp
    src/Cut.cpp
)
target_include_directories(audiocore PUBLIC include)
