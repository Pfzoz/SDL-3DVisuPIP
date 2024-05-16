#include <SDL.h>
#include <scene.hpp>

class Viewport
{
public:
    SDL_Texture *texture = NULL;
    SDL_Rect geometry;
    Viewport();
    ~Viewport();
    Viewport(int x, int y, int width, int height);
    void draw(SDL_Renderer *renderer);
};