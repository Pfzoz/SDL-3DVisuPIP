add_library(Camera STATIC
    camera.hpp
    camera.cpp)

target_include_directories(Camera PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Camera 
    PUBLIC
    Eigen3::Eigen) 