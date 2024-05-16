add_library(Canvas STATIC 
    canvas.hpp 
    canvas.cpp
    )

target_include_directories(Canvas PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Canvas  
    PUBLIC 
    Scene
    Primitives)