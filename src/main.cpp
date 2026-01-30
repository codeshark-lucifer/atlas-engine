#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <vulkan/vulkan.h>
#include <iostream>

const char *WINDOW_NAME = "atlas - engine";
const int WINDOW_WIDTH = 956;
const int WINDOW_HEIGHT = 540;

SDL_Window *window = nullptr;
bool running = true;

int main()
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        std::cerr << "Failed to initialize SDL3.\n";
        return EXIT_FAILURE;
    }

    SDL_WindowFlags flags = SDL_WINDOW_RESIZABLE;

    window = SDL_CreateWindow(
        WINDOW_NAME, WINDOW_WIDTH, WINDOW_HEIGHT,
        flags);
    SDL_assert(window);

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
            switch (event.type)
            {
            case SDL_EVENT_QUIT:
                running = false;
                break;
            default:
                break;
            }
        }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
