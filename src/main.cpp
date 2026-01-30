#include <vulkan/vulkan.h>
#include <SDL3/SDL.h>
#include <iostream>

const char* WINDOW_NAME = "atlas - engine";
const int WINDOW_WIDTH = 956;
const int WINDOW_HEIGHT = 540;

SDL_Window *window = nullptr;
bool running = true;

int main()
{
    if (!SDL_Init(SDL_INIT_VIDEO))
        SDL_Quit();

    SDL_WindowFlags flags = SDL_WINDOW_RESIZABLE;

    window = SDL_CreateWindow(
        WINDOW_NAME, WINDOW_WIDTH, WINDOW_HEIGHT,
        flags);

    if (!window)
    {
        std::cout << "Failed to create SDL Window." << std::endl;
        SDL_Quit();
    }

    SDL_Event event;
    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
                running = false;
        }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
