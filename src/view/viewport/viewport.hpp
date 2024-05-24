#include <SDL.h>
#include <scene.hpp>

class Viewport
{
public:
    SDL_Rect geometry;
    SDL_Texture *texture = NULL;
    Viewport();
    ~Viewport();
    Viewport(int x, int y, int width, int height);
    void draw(SDL_Renderer *renderer, SDL_Window *window);
};