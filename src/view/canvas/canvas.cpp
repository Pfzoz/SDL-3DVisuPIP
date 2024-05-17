#include "canvas.hpp"

Canvas::Canvas(){};
Canvas::Canvas(int x, int y, int width, int height)
{
    this->geometry = {x, y, width, height};
}

SDL_FPoint Canvas::normalize(int x, int y)
{
    float fx = ((float)x - (float)this->geometry.x) / (float)this->geometry.w;
    float fy = ((float)y - (float)this->geometry.y) / (float)this->geometry.h;
    return {fx, fy};
}

SDL_FPoint Canvas::unnormalize(float x, float y)
{
    float fx = (float)x * (float)this->geometry.w + (float)this->geometry.x;
    float fy = (float)y * (float)this->geometry.h + (float)this->geometry.y;
    return {fx, fy};
}

void Canvas::draw(SDL_Renderer *renderer)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &geometry);

    for (int i = 0; i < this->points.size(); i++)
    {
        SDL_FPoint unnormalized_point = this->unnormalize(this->points[i].x, this->points[i].y);
        SDL_DrawCircle(renderer, unnormalized_point.x, unnormalized_point.y, 7);
    }
    for (int i = 1; i < this->points.size(); i++)
    {
        SDL_FPoint p1 = this->unnormalize(this->points[i - 1].x, this->points[i - 1].y);
        SDL_FPoint p2 = this->unnormalize(this->points[i].x, this->points[i].y);
        SDL_RenderDrawLine(renderer, p1.x, p1.y, p2.x, p2.y);
    }
}

void Canvas::add_point(int x, int y)
{
    SDL_FPoint normalized_point = this->normalize(x, y);
    this->points.push_back(normalized_point);
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
        SDL_FPoint transformed_point;
        transformed_point.x = this->points[i].x * (float)this->l_width;
        transformed_point.y = (1 - this->points[i].y) * (float)this->l_height;
        transformed_points.push_back(transformed_point);
    }
    return Scene::create_wireframe(transformed_points, slices_amount);
}