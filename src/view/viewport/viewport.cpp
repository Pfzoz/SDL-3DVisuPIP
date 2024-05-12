#include "viewport.hpp"

Viewport::Viewport() {}

Viewport::Viewport(int x, int y, int width, int height)
{
    this->geometry = {x, y, width, height};
}

void Viewport::draw(SDL_Renderer *renderer)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &geometry);
}