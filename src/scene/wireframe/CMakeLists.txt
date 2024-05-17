add_library(Wireframe STATIC
    wireframe.hpp
    wireframe.cpp)

target_include_directories(Wireframe PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Wireframe
    PUBLIC
    Poly
    Utils
    Eigen3::Eigen
    SDL2::SDL2)