add_library(Canvas STATIC 
    canvas/canvas.hpp 
    canvas/canvas.cpp)

target_include_directories(Canvas PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/primitives)

target_link_libraries(Canvas  
    PUBLIC 
    Pipeline)

add_library(Primitives STATIC 
    primitives/primitives.hpp 
    primitives/primitives.cpp)

target_include_directories(Primitives PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/primitives)

target_link_libraries(Primitives  
    PUBLIC 
    SDL2::SDL2)

add_library(View STATIC 
    view.hpp 
    view.cpp)

target_include_directories(View PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(View  
    PUBLIC
    Canvas
    Primitives
    Utils
    SDL2::SDL2)