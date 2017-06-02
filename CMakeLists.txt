cmake_minimum_required(VERSION 2.8)
project(heatmap)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        main.cpp src/heat.h src/topython.h src/heatmap.py)

add_executable(heatmap ${SOURCE_FILES})

find_package (PythonLibs REQUIRED)
find_package (PythonInterp REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})
MESSAGE( STATUS "PYTHON_INCLUDE_DIRS: " ${PYTHON_INCLUDE_DIRS} )
target_link_libraries(heatmap ${PYTHON_LIBRARIES})
#Include Boost
FIND_PACKAGE(Boost COMPONENTS program_options REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(heatmap ${Boost_LIBRARIES})

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

configure_file(src/heatmap.py heatmap.py COPYONLY)
