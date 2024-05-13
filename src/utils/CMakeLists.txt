add_library(Utils STATIC
    utils.hpp
    utils.cpp)

target_include_directories(Utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Utils PUBLIC Eigen3::Eigen)
