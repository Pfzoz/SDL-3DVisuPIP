#ifndef DRAWING_H
#define DRAWING_H

#include <SDL.h>

void SDL_DrawCircle(SDL_Renderer *renderer, int32_t centreX, int32_t centreY, int32_t radius);

// Credits to stackoverflow user Scotty Stephens
// https://stackoverflow.com/questions/38334081/how-to-draw-circles-arcs-and-vector-graphics-in-sdl

#endif