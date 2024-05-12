add_library(Viewport STATIC 
    viewport.hpp 
    viewport.cpp
    )

target_include_directories(Viewport PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Viewport  
    PUBLIC 
    Pipeline
    SDL2::SDL2)