cmake_minimum_required(VERSION 3.27.7)
project(SDL-3DVisuPIP VERSION 1.0.0)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

find_package(Eigen3 3.3 REQUIRED NO_MODULE)

add_subdirectory(src)
