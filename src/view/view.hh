#ifndef VIEW_H
#define VIEW_H

#include "canvas.hh"
#include "../pipeline/modelling/gobject.hh"
#include "../utils.hh"

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

    void mouse_middle_down(int x, int y)
    {
        Wireframe wireframe = canvas.wireframe();
        GObject3d wireframeObject = wireframe.revolution();
        printf("== Wireframe Matrix ==\n");
        print_matrix(wireframeObject.get_hmatrix());
    }
};

#endif VIEW_H