#include "view.hpp"

// Constructor
View::View(int screen_width, int screen_height)
{
    this->canvas = Canvas(0, 0, screen_width, screen_height);
    this->canvas.set_logical_size(screen_width, screen_height);
}

// Rendering
void View::render(SDL_Renderer *renderer)
{
    this->canvas.render(renderer);
}

// Private Methods
/// Event Methods
void View::resize(float screen_width, float screen_height)
{
    this->canvas.geometry.h = screen_height * 0.95;
    this->canvas.geometry.w = screen_width;
    this->canvas.geometry.y = screen_height * 0.05;
}

void View::mouse_down(int x, int y)
{
    canvas.add_point(x, y);
}

void View::mouse_right_down(int x, int y)
{
    canvas.clear();
}

void View::mouse_middle_down(int x, int y)
{
    Poly::Polyhedron wireframe = canvas.get_wireframe();
    printf("== Wireframe Matrix ==\n");
    print_matrix(wireframe.get_hmatrix());
}

// Event Logic Handling
void View::handle_events(SDL_Event event)
{
    switch (event.type)
    {
    case SDL_MOUSEBUTTONDOWN:
        if (event.button.button == SDL_BUTTON_LEFT)
        {
            this->mouse_down(event.button.x, event.button.y);
        }
        else if (event.button.button == SDL_BUTTON_RIGHT)
        {
            this->mouse_right_down(event.button.x, event.button.y);
        }
        else if (event.button.button == SDL_BUTTON_MIDDLE)
        {
            this->mouse_middle_down(event.button.x, event.button.y);
        }
        break;
    case SDL_WINDOWEVENT:
        if (event.window.event == SDL_WINDOWEVENT_RESIZED)
        {
            this->resize(event.window.data1, event.window.data2);
        }
        break;
    }
}