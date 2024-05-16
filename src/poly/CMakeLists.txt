add_subdirectory(transformation)

add_library(Poly STATIC
    poly.hpp
    poly.cpp)

target_include_directories(Poly PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Poly
    PUBLIC
    Utils
    Transformation
    Eigen3::Eigen)