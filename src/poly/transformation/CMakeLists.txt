add_library(Transformation STATIC
    transformation.hpp
    transformation.cpp)

target_include_directories(Transformation PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Transformation
    PUBLIC
    Eigen3::Eigen)