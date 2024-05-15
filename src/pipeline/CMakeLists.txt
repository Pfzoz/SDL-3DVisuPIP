add_subdirectory(camera)

add_library(Pipeline STATIC 
    pipeline.hpp 
    pipeline.cpp)

target_include_directories(Pipeline PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Pipeline  
    PUBLIC
    Poly
    Utils
    Camera
    Eigen3::Eigen
    SDL2::SDL2)
