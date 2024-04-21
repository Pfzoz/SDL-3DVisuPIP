#ifndef VIEW_H
#define VIEW_H

#include "canvas.hh"

class View
{
private:
    Canvas canvas;

public:
    View(float screen_width, float screen_height)
    {
        this->canvas = Canvas(0, screen_height * 0.05, screen_width, screen_height * 0.95);
    }

    void render(SDL_Renderer *renderer)
    {
        this->canvas.render(renderer);
    }

    void resize(float screen_width, float screen_height)
    {
        this->canvas.geometry.h = screen_height * 0.95;
        this->canvas.geometry.w = screen_width;
        this->canvas.geometry.y = screen_height * 0.05;
    }

    void mouse_down(int x, int y)
    {
        canvas.add_point(x, y);
    }

    void mouse_right_down(int x, int y)
    {
        canvas.clear();
    }
};

#endif VIEW_H