#include "viewport.hpp"

Viewport::Viewport() {}
Viewport::~Viewport()
{
}

Viewport::Viewport(int x, int y, int width, int height)
{
    this->geometry = {x, y, width, height};
    Scene::Pipeline &pipeline = Scene::Pipeline::get_pipeline();
    pipeline.set_window({-960 * 2, -540 * 2, 960 * 2, 540 * 2});
    pipeline.set_srt(geometry);
}

void Viewport::draw(SDL_Renderer *renderer, SDL_Window *window)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    Scene::Pipeline &pipeline = Scene::Pipeline::get_pipeline();
    pipeline.render(renderer, window);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &geometry);
}