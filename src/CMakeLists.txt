include(FetchContent)
set(CMAKE_CXX_STANDARD 20)

FetchContent_Declare(
    SDL2
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-2.30.3
    FIND_PACKAGE_ARGS NAMES SDL2 REQUIRED
)
FetchContent_MakeAvailable(SDL2)

FetchContent_Declare(
    Eigen
    GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
    GIT_TAG 3.4.0
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
    FIND_PACKAGE_ARGS NAMES Eigen3 REQUIRED
)
set(BUILD_TESTING OFF)
set(EIGEN_BUILD_TESTING OFF)
set(EIGEN_MPL2_ONLY ON)
set(EIGEN_BUILD_PKGCONFIG OFF)
set(EIGEN_BUILD_DOC OFF)
FetchContent_MakeAvailable(Eigen)

FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG v1.90.6-docking
  FIND_PACKAGE_ARGS NAMES imgui REQUIRED
)
add_subdirectory(imgui)

add_subdirectory(utils)
add_subdirectory(poly)
add_subdirectory(scene)
add_subdirectory(view)

add_executable(
    SDL3VisuPIP
    main.hpp
    main.cpp)

target_link_libraries(
    SDL3VisuPIP
    PUBLIC
    View
    SDL2::SDL2
    imgui
)