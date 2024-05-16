#ifndef CANVAS_H
#define CANVAS_H

#include <vector>
#include <scene.hpp>
#include <primitives.hpp>

class Canvas
{
private:
public:
    double l_width = 1920, l_height = 1080;
    std::vector<SDL_FPoint> points;
    SDL_Rect geometry;

    Canvas();
    Canvas(int x, int y, int width, int height);
    void draw(SDL_Renderer *renderer);

    void add_point(int x, int y);

    void clear();
    void set_logical_size(int width, int height);

    SDL_FPoint normalize(int x, int y);
    SDL_FPoint unnormalize(float x, float y);

    Poly::Polyhedron get_wireframe(int slices_amount);
};

#endif