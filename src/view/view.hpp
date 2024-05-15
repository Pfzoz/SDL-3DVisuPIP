#ifndef VIEW_H
#define VIEW_H

#include <SDL.h>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include <utils.hpp>
#include <poly.hpp>
#include <viewport.hpp>
#include <canvas/canvas.hpp>
#include <pipeline.hpp>

enum class Scene
{
    Generatrix,
    Viewport
};

class View
{
private:
    // U.I Flags and Control Vars
    const float menu_height = 50;
    bool menu_generatrix_open = true;
    bool menu_camera_open = false;
    bool logical_size_follow_screen = false;

    // Drawing Vars
    SDL_Window* window = NULL;
    Scene scene_number = Scene::Generatrix;
    SDL_FPoint *dragging_point = NULL;
    time_t mouse_down_time = time(NULL);
    int screen_width, screen_height;
    int wireframe_slices_amount = 3;
    Canvas canvas;
    Viewport viewport;

    // Drawing - UI
    void draw_ui();
    void draw_generatrix_menu();
    void draw_point_position();
    void draw_camera_menu();

    // Drawing - Screen
    void drag_point(int mouse_x, int mouse_y);

    // Different event handling for different screens
    void handle_viewport_events(const SDL_Event *event);
    void handle_generatrix_events(const SDL_Event *event);

    // Events
    void resize(float screen_width, float screen_height);

    // Generatrix Events
    void generatrix_on_click(int x, int y);
    void generatrix_mouse_left_down(int x, int y);

    // SRT Events

    // Constructor
    View(SDL_Window* window, int width, int height);

public:
    // Singleton Instace
    static View &create_view(SDL_Window* window, int width, int height);

    // Main Rendering Loop
    void draw(SDL_Renderer *renderer);

    // Methods
    void handle_events(const SDL_Event *event);
};

#endif