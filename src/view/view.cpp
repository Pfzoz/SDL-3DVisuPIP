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
            if (ImGui::MenuItem("Generatrix", nullptr, this->scene_number == Screen::Generatrix))
            {
                scene_number = Screen::Generatrix;
                SDL_SetWindowTitle(this->window, "SDL3DVisuPIP - Generatrix");
            }
            if (ImGui::MenuItem("Viewport", nullptr, this->scene_number == Screen::Viewport))
            {
                scene_number = Screen::Viewport;
                SDL_SetWindowTitle(this->window, "SDL3DVisuPIP - Viewport");
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Windows"))
        {
            if (this->scene_number == Screen::Generatrix)
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
                if (ImGui::MenuItem("Objects Panel", nullptr, this->menu_objects_open))
                {
                    this->menu_objects_open = !this->menu_objects_open;
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
    if (this->scene_number == Screen::Generatrix)
    {
        draw_generatrix_menu();
        draw_point_position();
    }
    else
    {
        draw_camera_menu();
        draw_objects_menu();
    }
    if (this->logical_size_follow_screen)
    {
        canvas.l_width = this->screen_width;
        canvas.l_height = this->screen_height;
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
        int previous_width = canvas.l_width, previous_height = canvas.l_height;
        ImGui::InputDouble("L-Width", &canvas.l_width);
        ImGui::InputDouble("L-Height", &canvas.l_height);
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
            // wireframe.print_faces();
            pipeline.add_object(wireframe);
            object_names.clear();
            for (int i = 0; i < pipeline.scene_objects.size(); i++)
            {
                std::string object_name = "Object " + std::to_string(i);
                object_names.push_back(object_name);
            }
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
    if (this->menu_camera_open)
    {
        ImGui::SetNextWindowSizeConstraints({ImGui::CalcTextSize("Camera Options").x, 0.0f}, {-1, -1});
        ImGui::Begin("Camera Options", &this->menu_camera_open, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::SetWindowFontScale(1.4f);
        ImGui::SetWindowPos({0, menu_height}, ImGuiCond_FirstUseEver);
        ImGui::Text("VRP");
        double x, y, z;
        this->pipeline.get_vrp(&x, &y, &z);
        double input_x = x, input_y = y, input_z = z;
        double height_step = this->canvas.l_height / 100.0, height_fast_step = this->canvas.l_height / 10.0;
        double width_step = this->canvas.l_width / 100.0, width_fast_step = this->canvas.l_width / 10.0;
        ImGui::InputDouble("##SRCX", &input_x, width_step, width_fast_step, "%.6f...");
        ImGui::InputDouble("##SRCY", &input_y, height_step, height_fast_step, "%.6f...");
        ImGui::InputDouble("##SRCZ", &input_z, height_step, height_fast_step, "%.6f...");
        if (x != input_x || y != input_y || z != input_z)
        {
            this->pipeline.set_vrp(input_x, input_y, input_z);
        }
        ImGui::Text("Focal Point");
        this->pipeline.get_focal_point(&x, &y, &z);
        double input_fx = x, input_fy = y, input_fz = z;
        ImGui::InputDouble("##FPX", &input_fx, width_step, width_fast_step, "%.6f...");
        ImGui::InputDouble("##FPY", &input_fy, height_step, height_fast_step, "%.6f...");
        ImGui::InputDouble("##FPZ", &input_fz, height_step, height_fast_step, "%.6f...");
        if (x != input_fx || y != input_fy || z != input_fz)
        {
            this->pipeline.set_focal_point(input_fx, input_fy, input_fz);
        }
        ImGui::Text("Camera Orientation");
        this->pipeline.get_camera_view_up(&x, &y, &z);
        ImGui::Text("v (View Up)");
        ImGui::Text("X: %.6f Y: %.6f Z: %.6f", x, y, z);
        this->pipeline.get_camera_view_right(&x, &y, &z);
        ImGui::Text("u (View Side)");
        ImGui::Text("X: %.6f Y: %.6f Z: %.6f", x, y, z);
        this->pipeline.get_camera_view_direction(&x, &y, &z);
        ImGui::Text("n (View Direction)");
        ImGui::Text("X: %.6f Y: %.6f Z: %.6f", x, y, z);
        ImGui::End();
    }
}

void View::draw_objects_menu()
{
    if (this->menu_objects_open)
    {
        ImGui::SetNextWindowSizeConstraints({ImGui::CalcTextSize("Object Options").x, 0.0f}, {-1, -1});
        ImGui::Begin("Object Options", &this->menu_objects_open, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::SetWindowFontScale(1.4f);
        ImGui::SetWindowPos({0, menu_height}, ImGuiCond_FirstUseEver);
        ImGui::Text("Selected Object");
        std::vector<const char *> object_names_c;
        for (int i = 0; i < object_names.size(); i++)
            object_names_c.push_back(object_names[i].c_str());
        ImGui::Combo("##OBJDROPDOWN", &this->selected_object, object_names_c.data(), object_names.size());
        if (selected_object != -1)
        {
            ImGui::Text("Translate");
            ImGui::InputDouble("##TX", &tx, 0.01, 0.01, "%.6f...");
            ImGui::InputDouble("##TY", &ty, 0.01, 0.01, "%.6f...");
            ImGui::InputDouble("##TZ", &tz, 0.01, 0.01, "%.6f...");
            if (ImGui::Button("Translate"))
            {
                this->pipeline.translate_object(selected_object, tx, ty, tz);
            }
            ImGui::Text("Rotate");
            ImGui::Selectable("##LOCKPOSITIONROTATE", &lock_rotation_position);
            ImGui::SameLine();
            ImGui::Text("Lock Position");
            ImGui::InputDouble("##RX", &rx, 0.01, 0.01, "%.6f...");
            ImGui::InputDouble("##RY", &ry, 0.01, 0.01, "%.6f...");
            ImGui::InputDouble("##RZ", &rz, 0.01, 0.01, "%.6f...");
            ImGui::Selectable("##ANIMATEROTATE", &rotation_animate, 0, {50, 0});
            ImGui::SameLine();
            ImGui::Text("Animate");
            double delta = SDL_GetTicks64() - last_time;
            if (ImGui::Button("Rotate"))
            {
                if (!rotation_animate)
                {
                    if (!lock_rotation_position)
                        this->pipeline.rotate_object(selected_object, rx, ry, rz);
                    else
                    {
                        double x, y, z;
                        this->pipeline.get_object_center(selected_object, &x, &y, &z);
                        this->pipeline.translate_object(selected_object, -x, -y, -z);
                        this->pipeline.rotate_object(selected_object, rx, ry, rz);
                        this->pipeline.translate_object(selected_object, x, y, z);
                    }
                }
            }
            else if (rotation_animate)
            {
                double x, y, z;
                this->pipeline.get_object_center(selected_object, &x, &y, &z);
                this->pipeline.translate_object(selected_object, -x, -y, -z);
                this->pipeline.rotate_object(selected_object, rx / delta, ry / delta, rz / delta);
                this->pipeline.translate_object(selected_object, x, y, z);
            }
            if (ImGui::Button("Delete"))
            {
                this->pipeline.remove_object(selected_object);
                this->selected_object = -1;
                object_names.clear();
                for (int i = 0; i < pipeline.scene_objects.size(); i++)
                {
                    std::string object_name = "Object " + std::to_string(i);
                    object_names.push_back(object_name);
                }
            }
        }
        ImGui::End();
    }
}

// Drawing - Scenes
void View::draw(SDL_Renderer *renderer)
{
    // U.I Drawing (Comes first so events work correctly)
    this->draw_ui();
    // Screen Drawing
    if (scene_number == Screen::Generatrix)
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
    const Uint8 *keystate = SDL_GetKeyboardState(NULL);
    if (keystate[SDL_SCANCODE_LCTRL] || keystate[SDL_SCANCODE_RCTRL])
    {
        for (int i = 0; i < this->canvas.points.size(); i++)
        {
            double lock_in_range = 0.01;
            if (this->canvas.points[i].x > normalized_point.x - lock_in_range && this->canvas.points[i].x<normalized_point.x + lock_in_range &&this->canvas.points[i].y> normalized_point.y - lock_in_range && this->canvas.points[i].y < normalized_point.y + lock_in_range)
            {
                this->dragging_point->x = this->canvas.points[i].x;
                this->dragging_point->y = this->canvas.points[i].y;
                break;
            }
        }
    }
    else
    {
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
    if (this->scene_number == Screen::Generatrix)
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