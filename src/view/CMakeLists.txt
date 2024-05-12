add_subdirectory(primitives)
add_subdirectory(canvas)
add_subdirectory(viewport)

add_library(View STATIC 
    view.hpp 
    view.cpp)

target_include_directories(View PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(View  
    PUBLIC
    Canvas
    Primitives
    Utils
    Viewport
    SDL2::SDL2
    imgui)