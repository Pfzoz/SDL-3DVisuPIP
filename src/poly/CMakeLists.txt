add_library(Transformation STATIC
    transformation/transformation.hpp
    transformation/transformation.cpp)

target_include_directories(Transformation PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/transformation)

target_link_libraries(Transformation
    PRIVATE
    Eigen3::Eigen)

add_library(Poly STATIC
    poly.hpp
    poly.cpp)

target_include_directories(Poly PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Poly
    PRIVATE
    Transformation
    Eigen3::Eigen)