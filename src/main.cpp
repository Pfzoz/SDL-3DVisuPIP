#include "main.hpp"

// Globals
int screen_width, screen_height;
SDL_Window *main_window;
SDL_Renderer *renderer;
ImGuiIO io;

// Initialization
bool init_SDL()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("Could not initialize SDL_VIDEO!\n");
        printf("SDL_Error: %s\n", SDL_GetError());
        return false;
    }
    main_window = SDL_CreateWindow("SDL3DVisuPIP - Generatrix", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_width, screen_height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED);
    if (!main_window)
    {
        printf("Window(s) could not be created.\n");
        printf("SDL_Error: %s\n", SDL_GetError());
        return false;
    }
    renderer = SDL_CreateRenderer(main_window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        printf("Renderer could not be created.\n");
        printf("SDL_Error: %s\n", SDL_GetError());
        return false;
    }
    return true;
}

void init_imgui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io = {ImGui::GetIO()};
    ImGui_ImplSDL2_InitForSDLRenderer(main_window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
}

// Main
int main(int argc, char *argv[])
{
    if (!init_SDL())
        return 1;
    init_imgui();

    SDL_DisplayMode DM;
    SDL_GetCurrentDisplayMode(0, &DM);
    screen_width = DM.w;
    screen_height = DM.h;

    SDL_SetWindowMinimumSize(main_window, 300, 300);

    View view = View::create_view(main_window, screen_width, screen_height);

    bool quit = false;
    SDL_Event event;

    while (!quit)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                quit = true;
            ImGui_ImplSDL2_ProcessEvent(&event);
            view.handle_events(&event);
        }
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        // Imgui Frame
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // Main Drawing
        view.draw(renderer);

        // Rendering
        ImGui::Render();
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
        SDL_RenderPresent(renderer);
    }
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_DestroyWindow(main_window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
}