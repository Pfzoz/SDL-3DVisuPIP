#include <SDL2/SDL.h>
#include "view/view.hh"

float screen_width = 640, screen_height = 480;

int main()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("Could not initialize SDL_VIDEO!\n");
        printf("SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Hello World", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_width, screen_height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, 1, SDL_RENDERER_ACCELERATED);

    View view = View(screen_width, screen_height);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    bool quit = false;
    SDL_Event event;

    while (!quit)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                quit = true;
            }
            else if (event.type == SDL_WINDOWEVENT)
            {
                if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    screen_width = event.window.data1;
                    screen_height = event.window.data2;
                    view.resize(screen_width, screen_height);
                }
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                if (event.button.button == SDL_BUTTON_LEFT)
                    view.mouse_down(event.button.x, event.button.y);
                else if (event.button.button == SDL_BUTTON_RIGHT)
                    view.mouse_right_down(event.button.x, event.button.y);
            }
        }
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        view.render(renderer);

        SDL_RenderPresent(renderer);
    }

    SDL_Quit();
}