#ifndef CANVAS_H
#define CANVAS_H

#include <SDL2/SDL_main.h>
#include <vector>
#include "../../pipeline/pipeline.hpp"
#include "../primitives/primitives.hpp"
#include <eigen3/Eigen/Core>

class Canvas
{
private:
    int l_width = 1920, l_height = 1080;

public:
    std::vector<SDL_FPoint> points;
    SDL_Rect geometry;

    Canvas();
    Canvas(int x, int y, int width, int height);
    void render(SDL_Renderer *renderer);

    void add_point(int x, int y);

    void clear();
    void set_logical_size(int width, int height);

    Poly::Polyhedron get_wireframe();
};

#endif