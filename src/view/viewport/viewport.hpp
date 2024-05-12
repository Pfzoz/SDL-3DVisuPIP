#include <SDL.h>

class Viewport
{
public:
    SDL_Rect geometry;
    Viewport();
    Viewport(int x, int y, int width, int height);
    void draw(SDL_Renderer *renderer);
};