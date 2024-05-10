add_library(Utils STATIC
    utils.hpp
    utils.cpp)

target_include_directories(Utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(poly)
add_subdirectory(pipeline)
add_subdirectory(view)

add_executable(SDL3VisuPIP.x main.cpp)

target_link_libraries(
    SDL3VisuPIP.x
    PUBLIC
    View
    SDL2::SDL2
)