#include "viewport.hpp"

Viewport::Viewport() {}
Viewport::~Viewport()
{
    if (texture != NULL)
        SDL_DestroyTexture(texture);
}

Viewport::Viewport(int x, int y, int width, int height)
{
    this->geometry = {x, y, width, height};
    Scene::Pipeline &pipeline = Scene::Pipeline::get_pipeline();
    pipeline.set_window({-960 * 2, -540 * 2, 960 * 2, 540 * 2});
    pipeline.set_srt(geometry);
}

void Viewport::draw(SDL_Renderer *renderer)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    Scene::Pipeline &pipeline = Scene::Pipeline::get_pipeline();
    if (pipeline.is_altered())
    {
        if (texture != NULL)
            SDL_DestroyTexture(texture);
        this->texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, geometry.w, geometry.h);
        pipeline.apply(renderer, texture);
        SDL_RenderDrawRect(renderer, &geometry);
        SDL_RenderCopy(renderer, texture, NULL, &geometry);
    }
}