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
                if (ImGui::MenuItem("Projection Panel", nullptr, this->menu_projection_open))
                {
                    this->menu_projection_open = !this->menu_projection_open;
                }
                if (ImGui::MenuItem("Shading Panel", nullptr, this->menu_shading_open))
                {
                    this->menu_shading_open = !this->menu_shading_open;
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
        draw_projection_menu();
        draw_shading_menu();
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
        ImGui::Text("Transform");
        ImGui::InputDouble("Translate Step", &this->camera_translate_step, 0.01, 0.01, "%.6f...");
        if (ImGui::Button("T(X)"))
            this->pipeline.translate_camera(this->camera_translate_step, 0, 0);
        ImGui::SameLine();
        if (ImGui::Button("T(Y)"))
            this->pipeline.translate_camera(0, this->camera_translate_step, 0);
        ImGui::SameLine();
        if (ImGui::Button("T(Z)"))
            this->pipeline.translate_camera(0, 0, this->camera_translate_step);
        ImGui::InputDouble("Rotate Step", &this->camera_rotate_step, 0.01, 0.01, "%.6f...");
        if (ImGui::Button("R(X)"))
            this->pipeline.rotate_camera(this->camera_rotate_step * (M_PI / 180.0), 0, 0);
        ImGui::SameLine();
        if (ImGui::Button("R(Y)"))
            this->pipeline.rotate_camera(0, this->camera_rotate_step * (M_PI / 180.0), 0);
        ImGui::SameLine();
        if (ImGui::Button("R(Z)"))
            this->pipeline.rotate_camera(0, 0, this->camera_rotate_step * (M_PI / 180.0));
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
            ImGui::Text("Rotate (Degrees)");
            ImGui::Selectable("##LOCKPOSITIONROTATE", &lock_rotation_position);
            ImGui::SameLine();
            ImGui::Text("Lock Position");
            double previous_rx = rx, previous_ry = ry, previous_rz = rz;
            ImGui::InputDouble("##RX", &rx, 0.01, 0.01, "%.6f...");
            ImGui::InputDouble("##RY", &ry, 0.01, 0.01, "%.6f...");
            ImGui::InputDouble("##RZ", &rz, 0.01, 0.01, "%.6f...");
            if (rx != previous_rx || ry != previous_ry || rz != previous_rz)
            {
                radians_x = rx * (M_PI / 180.0);
                radians_y = ry * (M_PI / 180.0);
                radians_z = rz * (M_PI / 180.0);
            }
            ImGui::Selectable("##ANIMATEROTATE", &rotation_animate, 0, {50, 0});
            ImGui::SameLine();
            ImGui::Text("Animate");
            double delta = SDL_GetTicks64() - last_time;
            if (ImGui::Button("Rotate"))
            {
                if (!rotation_animate)
                {
                    if (!lock_rotation_position)
                        this->pipeline.rotate_object(selected_object, radians_x, radians_y, radians_z);
                    else
                    {
                        double x, y, z;
                        this->pipeline.get_object_center(selected_object, &x, &y, &z);
                        this->pipeline.translate_object(selected_object, -x, -y, -z);
                        this->pipeline.rotate_object(selected_object, radians_x, radians_y, radians_z);
                        this->pipeline.translate_object(selected_object, x, y, z);
                    }
                }
            }
            else if (rotation_animate)
            {
                double x, y, z;
                this->pipeline.get_object_center(selected_object, &x, &y, &z);
                this->pipeline.translate_object(selected_object, -x, -y, -z);
                this->pipeline.rotate_object(selected_object, radians_x / delta, radians_y / delta, radians_z / delta);
                this->pipeline.translate_object(selected_object, x, y, z);
            }
            ImGui::Text("Scale");
            ImGui::InputDouble("##SX", &sx, 0.01, 0.01, "%.6f...");
            ImGui::InputDouble("##SY", &sy, 0.01, 0.01, "%.6f...");
            ImGui::InputDouble("##SZ", &sz, 0.01, 0.01, "%.6f...");
            if (ImGui::Button("Scale"))
            {
                this->pipeline.scale_object(selected_object, sx, sy, sz);
            }
            ImGui::Text("Material");
            double specular_exponent = this->pipeline.get_object_specular_exponent(selected_object);
            double pspecular_exponent = specular_exponent;
            double ac_r, ac_g, ac_b, dc_r, dc_g, dc_b, sc_r, sc_g, sc_b;
            double pac_r, pac_g, pac_b, pdc_r, pdc_g, pdc_b, psc_r, psc_g, psc_b;
            this->pipeline.get_object_ambient_coefficients(selected_object, &ac_r, &ac_g, &ac_b);
            this->pipeline.get_object_diffuse_coefficients(selected_object, &dc_r, &dc_g, &dc_b);
            this->pipeline.get_object_specular_coefficients(selected_object, &sc_r, &sc_g, &sc_b);
            ImGui::Text("Ambient Lighting Coefficients");
            ImGui::InputDouble("KaR", &ac_r, 0.01, 0.01, "%.6f...");
            ImGui::SameLine();
            ImGui::InputDouble("KaG", &ac_g, 0.01, 0.01, "%.6f...");
            ImGui::SameLine();
            ImGui::InputDouble("KaB", &ac_b, 0.01, 0.01, "%.6f...");
            ImGui::Text("Diffuse Reflection Coefficients");
            ImGui::InputDouble("KdR", &dc_r, 0.01, 0.01, "%.6f...");
            ImGui::SameLine();
            ImGui::InputDouble("KdG", &dc_g, 0.01, 0.01, "%.6f...");
            ImGui::SameLine();
            ImGui::InputDouble("KdB", &dc_b, 0.01, 0.01, "%.6f...");
            ImGui::Text("Specular Reflection Coefficients");
            ImGui::InputDouble("KsR", &sc_r, 0.01, 0.01, "%.6f...");
            ImGui::SameLine();
            ImGui::InputDouble("KsG", &sc_g, 0.01, 0.01, "%.6f...");
            ImGui::SameLine();
            ImGui::InputDouble("KsB", &sc_b, 0.01, 0.01, "%.6f...");
            ImGui::Text("Specular Exponent");
            ImGui::InputDouble("KsE", &pspecular_exponent, 0.01, 0.01, "%.6f...");
            this->pipeline.get_object_ambient_coefficients(selected_object, &pac_r, &pac_g, &pac_b);
            this->pipeline.get_object_diffuse_coefficients(selected_object, &pdc_r, &pdc_g, &pdc_b);
            this->pipeline.get_object_specular_coefficients(selected_object, &psc_r, &psc_g, &psc_b);
            if (pspecular_exponent != specular_exponent)
                this->pipeline.set_object_specular_exponent(selected_object, pspecular_exponent);
            if (pac_r != ac_r || pac_g != ac_g || pac_b != ac_b || pdc_r != dc_r || pdc_g != dc_g || pdc_b != dc_b ||
                psc_r != sc_r || psc_g != sc_g || psc_b != sc_b)
            {
                this->pipeline.set_object_ambient_coefficients(selected_object, ac_r, ac_g, ac_b);
                this->pipeline.set_object_diffuse_coefficients(selected_object, dc_r, dc_g, dc_b);
                this->pipeline.set_object_specular_coefficients(selected_object, sc_r, sc_g, sc_b);
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

void View::draw_projection_menu()
{
    if (this->menu_projection_open)
    {
        ImGui::SetNextWindowSizeConstraints({ImGui::CalcTextSize("Projection Options").x, 0.0f}, {-1, -1});
        ImGui::Begin("Projection Options", &this->menu_projection_open, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::SetWindowFontScale(1.4f);
        ImGui::SetWindowPos({0, menu_height}, ImGuiCond_FirstUseEver);
        std::vector<const char *> projection_names_c;
        projection_names_c.push_back("Parallel");
        projection_names_c.push_back("Perspective");
        int previous_selection = this->selected_projection;
        ImGui::Text("Projection Type");
        ImGui::Combo("##PROJDROPDOWN", &this->selected_projection, projection_names_c.data(), projection_names_c.size());
        if (previous_selection != this->selected_projection)
        {
            Scene::Projection projection;
            switch (selected_projection)
            {
            case 0:
                projection = Scene::Projection::PARALLEL;
                break;
            case 1:
                projection = Scene::Projection::PERSPECTIVE;
                break;
            }
            this->pipeline.use_projection(projection);
        }
        ImGui::Text("Focal Length");
        double previous_focal_length = this->focal_length;
        ImGui::InputDouble("##FOCAL", &this->focal_length, 0.01, 0.01, "%.6f...");
        if (previous_focal_length != this->focal_length)
        {
            this->pipeline.set_projection_distance(this->focal_length);
        }
        ImGui::End();
    }
}

void View::draw_shading_menu()
{
    if (this->menu_shading_open)
    {
        ImGui::SetNextWindowSizeConstraints({ImGui::CalcTextSize("Shading Options").x, 0.0f}, {-1, -1});
        ImGui::Begin("Shading Options", &this->menu_shading_open, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::SetWindowFontScale(1.4f);
        ImGui::SetWindowPos({0, menu_height}, ImGuiCond_FirstUseEver);
        if (ImGui::Selectable("Use Normal Visibility", this->pipeline.using_painter_clipper()))
        {
            this->pipeline.use_painter_clip(!this->pipeline.using_painter_clipper());
        }
        if (ImGui::RadioButton("Wireframe",
                               this->pipeline.get_shading() == Scene::Shading::NO_SHADING && !this->pipeline.using_z_buffer()))
        {
            this->pipeline.use_zbuffer(false);
            this->pipeline.use_shading(Scene::Shading::NO_SHADING);
        }
        if (ImGui::RadioButton("Wireframe & Z-Buffer",
                               this->pipeline.get_shading() == Scene::Shading::NO_SHADING && this->pipeline.using_z_buffer()))
        {
            this->pipeline.use_zbuffer(true);
            this->pipeline.use_shading(Scene::Shading::NO_SHADING);
        }
        double l_x, l_y, l_z;
        this->pipeline.get_lights_position(&l_x, &l_y, &l_z);
        double pl_x = l_x, pl_y = l_y, pl_z = l_z;
        ImGui::Text("Lights Position");
        ImGui::InputDouble("X", &pl_x, 0.01, 0.01, "%.6f...");
        ImGui::SameLine();
        ImGui::InputDouble("Y", &pl_y, 0.01, 0.01, "%.6f...");
        ImGui::SameLine();
        ImGui::InputDouble("Z", &pl_z, 0.01, 0.01, "%.6f...");
        if (pl_x != l_x || pl_y != l_y || pl_z != l_z)
            this->pipeline.set_lights_position(pl_x, pl_y, pl_z);
        ImGui::Text("Ambient Light");
        double ia_r, ia_g, ia_b, ii_r, ii_g, ii_b;
        this->pipeline.get_ambient_light_intensity(&ia_r, &ia_g, &ia_b);
        this->pipeline.get_illumination_intensity(&ii_r, &ii_g, &ii_b);
        ImGui::InputDouble("IaR", &ia_r, 0.01, 0.01, "%.6f...");
        ImGui::SameLine();
        ImGui::InputDouble("IaG", &ia_g, 0.01, 0.01, "%.6f...");
        ImGui::SameLine();
        ImGui::InputDouble("IaB", &ia_b, 0.01, 0.01, "%.6f...");
        ImGui::Text("Illumination");
        ImGui::InputDouble("IiR", &ii_r, 0.01, 0.01, "%.6f...");
        ImGui::SameLine();
        ImGui::InputDouble("IiG", &ii_g, 0.01, 0.01, "%.6f...");
        ImGui::SameLine();
        ImGui::InputDouble("IiB", &ii_b, 0.01, 0.01, "%.6f...");
        double pia_r, pia_g, pia_b, pii_r, pii_g, pii_b;
        this->pipeline.get_ambient_light_intensity(&pia_r, &pia_g, &pia_b);
        this->pipeline.get_illumination_intensity(&pii_r, &pii_g, &pii_b);
        if (ia_r != pia_r || ia_g != pia_g || ia_b != pia_b || ii_r != pii_r || ii_g != pii_g || ii_b != pii_b)
        {
            this->pipeline.set_ambient_light(ia_r, ia_g, ia_b);
            this->pipeline.set_lights_intensity(ii_r, ii_g, ii_b);
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
        this->viewport.draw(renderer, window);
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