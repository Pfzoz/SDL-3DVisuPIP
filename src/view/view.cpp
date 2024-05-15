#include <view.hpp>

// Singleton

View &View::create_view(SDL_Window *window, int width, int height)
{
    static View instance(window, width, height);
    return instance;
}

// Constructor
View::View(SDL_Window *window, int screen_width, int screen_height)
{
    this->window = window;
    this->screen_width = screen_width;
    this->screen_height = screen_height;
    this->canvas = Canvas(screen_width * 0.01, menu_height + screen_height * 0.01,
                          screen_width - screen_width * 0.01, screen_height - menu_height - screen_height * 0.01);
    this->canvas.set_logical_size(screen_width, screen_height);
    this->viewport = Viewport(0, menu_height, screen_width, screen_height - menu_height);
}

// Drawing - UI
void View::draw_ui()
{
    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_MenuBar;
    // window_flags |= ImGuiWindowFlags_Popup;
    ImGui::Begin("Menu", nullptr, window_flags);
    ImGui::SetWindowFontScale(1.5f);
    ImGui::SetWindowSize({(float)screen_width, menu_height});
    ImGui::SetWindowPos({0, 0});
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Screen"))
        {
            if (ImGui::MenuItem("Generatrix", nullptr, this->scene_number == Scene::Generatrix))
            {
                scene_number = Scene::Generatrix;
                SDL_SetWindowTitle(this->window, "SDL3DVisuPIP - Generatrix");
            }
            if (ImGui::MenuItem("Viewport", nullptr, this->scene_number == Scene::Viewport))
            {
                scene_number = Scene::Viewport;
                SDL_SetWindowTitle(this->window, "SDL3DVisuPIP - Viewport");
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Windows"))
        {
            if (this->scene_number == Scene::Generatrix)
            {
                if (ImGui::MenuItem("Generatrix Panel", nullptr, this->menu_generatrix_open))
                {
                    this->menu_generatrix_open = !this->menu_generatrix_open;
                }
            }
            else
            {
                if (ImGui::MenuItem("Camera Panel", nullptr, this->menu_camera_open))
                {
                    this->menu_camera_open = !this->menu_camera_open;
                }
            }
            ImGui::EndMenu();
        }
        if (ImGui::Button("Exit"))
        {
            SDL_Event event = {SDL_QUIT};
            SDL_PushEvent(&event);
        }
        ImGui::EndMenuBar();
    }
    ImGui::End();
    if (this->scene_number == Scene::Generatrix)
    {
        draw_generatrix_menu();
        draw_point_position();
    }
    else
    {
        draw_camera_menu();
    }
}

void View::draw_generatrix_menu()
{
    if (this->menu_generatrix_open)
    {
        ImGui::Begin("Generatrix Options", &this->menu_generatrix_open, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::SetWindowFontScale(1.4f);
        ImGui::SetWindowCollapsed(false, ImGuiCond_FirstUseEver);
        ImGui::SetWindowPos({0, menu_height}, ImGuiCond_FirstUseEver);
        ImGui::Text("Logical Size (Min = 1)");
        ImGui::AlignTextToFramePadding();
        ImGui::Checkbox("Follow Screen Resolution", &this->logical_size_follow_screen);
        if (this->logical_size_follow_screen)
        {
            canvas.l_width = this->screen_width;
            canvas.l_height = this->screen_height;
        }
        int previous_width = canvas.l_width, previous_height = canvas.l_height;
        ImGui::InputDouble("Width", &canvas.l_width);
        ImGui::InputDouble("Height", &canvas.l_height);
        if (previous_width != canvas.l_width || previous_height != canvas.l_height)
            this->logical_size_follow_screen = false;
        if (!canvas.l_width)
            canvas.l_width = 1;
        if (!canvas.l_height)
            canvas.l_height = 1;
        ImGui::Text("Amount of Slices For Revolution (Min = 3)");
        ImGui::InputInt("Slices", &this->wireframe_slices_amount);
        if (this->wireframe_slices_amount < 3)
            this->wireframe_slices_amount = 3;
        if (ImGui::Button("Create Wireframe (Send to 3D)", {ImGui::GetContentRegionAvail().x, ImGui::GetFontSize() * 1.25f}))
        {
            Poly::Polyhedron wireframe = canvas.get_wireframe(this->wireframe_slices_amount);
        }
        if (ImGui::Button("Clear Points", {ImGui::GetContentRegionAvail().x, ImGui::GetFontSize() * 1.25f}))
            canvas.clear();
        ImGui::End();
    }
}

void View::draw_point_position()
{
    if (this->dragging_point != NULL)
    {
        ImGui::Begin("Point Coordinates", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
        ImGui::SetWindowFontScale(1.5f);
        ImGui::Text("X: %f", this->dragging_point->x);
        ImGui::Text("Y: %f", this->dragging_point->y);
        float win_x, win_y = this->screen_height - ImGui::GetWindowSize().y;
        if (this->dragging_point->x > 0.5f)
            win_x = 0;
        else
            win_x = this->screen_width - ImGui::GetWindowSize().x;
        ImGui::SetWindowPos({win_x, win_y});
        ImGui::End();
    }
}

void View::draw_camera_menu()
{
    ImGui::Begin("Camera Options", &this->menu_camera_open, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::SetWindowPos({0, menu_height}, ImGuiCond_FirstUseEver);
    ImGui::End();
}

// Drawing - Scenes
void View::draw(SDL_Renderer *renderer)
{
    // U.I Drawing (Comes first so events work correctly)
    this->draw_ui();
    // Screen Drawing
    if (scene_number == Scene::Generatrix)
    {
        if (this->dragging_point != NULL)
        {
            int mouse_x, mouse_y;
            SDL_GetMouseState(&mouse_x, &mouse_y);
            this->drag_point(mouse_x, mouse_y);
        }
        this->canvas.draw(renderer);
    }
    else
    {
        this->viewport.draw(renderer);
    }
}

void View::drag_point(int mouse_x, int mouse_y)
{
    SDL_FPoint normalized_point = this->canvas.normalize(mouse_x, mouse_y);
    mouse_y = mouse_y - this->canvas.geometry.y;
    mouse_x = mouse_x - this->canvas.geometry.x;
    if (mouse_x < 0)
        this->dragging_point->x = 0;
    else if (mouse_x > this->canvas.geometry.w)
        this->dragging_point->x = 1;
    else
        this->dragging_point->x = normalized_point.x;
    if (mouse_y < 0)
        this->dragging_point->y = 0;
    else if (mouse_y > this->canvas.geometry.h)
        this->dragging_point->y = 1;
    else
        this->dragging_point->y = normalized_point.y;
}

// Events
void View::resize(float screen_width, float screen_height)
{
    this->screen_width = screen_width;
    this->screen_height = screen_height;
    this->canvas.geometry = {
        (int)(screen_width * 0.01) / 2,
        (int)(menu_height + (screen_height * 0.01) / 2),
        (int)(screen_width - (screen_width * 0.01)),
        (int)(screen_height - menu_height - (screen_height * 0.01))};
}

// Generatrix Events
void View::generatrix_on_click(int x, int y)
{
    canvas.add_point(x, y);
}

void View::generatrix_mouse_left_down(int x, int y)
{
    for (int i = 0; i < this->canvas.points.size(); i++)
    {
        SDL_FPoint touch_point = this->canvas.unnormalize(this->canvas.points[i].x, this->canvas.points[i].y);
        if (touch_point.x <= x + 7 && touch_point.x >= x - 7 &&
            touch_point.y <= y + 7 && touch_point.y >= y - 7)
        {
            this->dragging_point = &this->canvas.points[i];
            break;
        }
    }
}

// Viewport Events

// Event Logic Handling
void View::handle_events(const SDL_Event *event)
{
    // Handles depending on current scene
    if (this->scene_number == Scene::Generatrix)
        this->handle_generatrix_events(event);
    else
        this->handle_viewport_events(event);
    if (event->type == SDL_WINDOWEVENT)
    {
        if (event->window.event == SDL_WINDOWEVENT_RESIZED)
        {
            this->resize(event->window.data1, event->window.data2);
        }
    }
}

// Handle only Generatrix Scene Events
void View::handle_generatrix_events(const SDL_Event *event)
{
    switch (event->type)
    {
    case SDL_MOUSEBUTTONDOWN:
        if (ImGui::GetIO().WantCaptureMouse)
            break;
        if (event->button.button == SDL_BUTTON_LEFT)
        {
            this->mouse_down_time = time(NULL);
            this->generatrix_mouse_left_down(event->button.x, event->button.y);
        }
        break;
    case SDL_MOUSEBUTTONUP:
        if (ImGui::GetIO().WantCaptureMouse)
            break;
        if (event->button.button == SDL_BUTTON_LEFT)
        {
            if (time(NULL) - this->mouse_down_time < 0.35 && this->dragging_point == NULL)
            {
                this->generatrix_on_click(event->button.x, event->button.y);
            }
            this->dragging_point = NULL;
        }
        break;
    }
}

// Handle Only Viewport Scene Events
void View::handle_viewport_events(const SDL_Event *event) {}