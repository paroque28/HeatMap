cmake_minimum_required(VERSION 3.6)
project(heatmap)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        main.cpp src/Liebmann.h)

add_executable(heatmap ${SOURCE_FILES})

#Include Boost
FIND_PACKAGE(Boost COMPONENTS program_options REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(heatmap ${Boost_LIBRARIES})