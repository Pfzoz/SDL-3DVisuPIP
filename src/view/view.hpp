#ifndef VIEW_H
#define VIEW_H

#include "../utils.hpp"
#include "../poly/poly.hpp"
#include "canvas/canvas.hpp"

class View
{
private:
    Canvas canvas;
    
    // Events
    void resize(float screen_width, float screen_height);
    void mouse_down(int x, int y);
    void mouse_right_down(int x, int y);
    void mouse_middle_down(int x, int y);

public:

    // Constructor
    View(float screen_width, float screen_height);
    
    // Main Rendering Loop
    void render(SDL_Renderer *renderer);

    // Methods
    void handle_events(SDL_Event event);
};

#endif