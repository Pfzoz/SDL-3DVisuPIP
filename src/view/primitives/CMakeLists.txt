add_library(Primitives STATIC 
    primitives.hpp 
    primitives.cpp)

target_include_directories(Primitives PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Primitives  
    PUBLIC
    SDL2::SDL2)