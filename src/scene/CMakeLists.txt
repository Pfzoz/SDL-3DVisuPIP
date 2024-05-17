add_subdirectory(camera)
add_subdirectory(wireframe)

add_library(Scene STATIC 
    scene.hpp 
    scene.cpp)

target_include_directories(Scene PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Scene
    PUBLIC
    Poly
    Utils
    Camera
    Wireframe
    Eigen3::Eigen
    SDL2::SDL2)
