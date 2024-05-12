#include "canvas.hpp"

Canvas::Canvas(){};
Canvas::Canvas(int x, int y, int width, int height)
{
    this->geometry = {x, y, width, height};
}

void Canvas::draw(SDL_Renderer *renderer)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &geometry);

    for (int i = 0; i < this->points.size(); i++)
    {
        SDL_DrawCircle(renderer, this->points[i].x * this->geometry.w, this->points[i].y * this->geometry.h, 7);
    }
    for (int i = 1; i < this->points.size(); i++)
    {
        SDL_RenderDrawLine(renderer, this->points[i - 1].x * this->geometry.w, this->points[i - 1].y * this->geometry.h, this->points[i].x * this->geometry.w, this->points[i].y * this->geometry.h);
    }
}

void Canvas::add_point(int x, int y)
{
    float fx = (float)x / (float)this->geometry.w;
    float fy = (float)y / (float)this->geometry.h;
    this->points.push_back({fx, fy});
}

void Canvas::clear()
{
    this->points.clear();
}

void Canvas::set_logical_size(int width, int height)
{
    this->l_width = width;
    this->l_height = height;
}

Poly::Polyhedron Canvas::get_wireframe(int slices_amount)
{
    std::vector<SDL_FPoint> transformed_points;
    
    
    for (int i = 0; i < this->points.size(); i++)
    {
        transformed_points.push_back(SDL_FPoint{this->points[i].x * (float)this->l_width, this->points[i].y * (float)this->l_height});
    }
    return Pip::wireframe(transformed_points, slices_amount);
}