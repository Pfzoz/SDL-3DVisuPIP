cmake_minimum_required(VERSION 3.27.7)
project(SDL-3DVisuPIP VERSION 1.0.0)

if(LINUX)
    include(FetchContent)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
    message("Finding SDL2...")
    find_package(SDL2 REQUIRED)
    if(NOT SDL2_FOUND)
        message("SDL2 NOT FOUND.")
        message("Fetching SDL...")
        FetchContent_Declare(
            SDL2
            GIT_REPOSITORY "https://github.com/libsdl-org/SDL.git"
            GIT_TAG release-2.30.3)
      add_subdirectory(sdl)
    else()
        message("SDL2 FOUND.")
        include_directories(SDL::SDL2main)
    endif()
    message("Finding Eigen3...")
    find_package(Eigen3 3.4 REQUIRED)
    if(NOT EIGEN3_FOUND)
        message("Eigen3 NOT FOUND.")
        message("Fetching Eidgen3...")
        FetchContent_Declare(
            SDL2
            GIT_REPOSITORY "https://gitlab.com/libeigen/eigen.git"
            GIT_TAG release-3.4.0)
        add_subdirectory(eigen)
    else()
        message("Eigen3 FOUND.")
        include_directories(Eigen3::Eigen)
    endif()
elseif(WIN32)
    message("Windows")
elseif(APPLE)
    message("MacOS")
else()
    message("Unsupported System")
endif()

add_subdirectory(src)
