#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <iostream>
#include <vector>

#include <engine/scene.hpp>
#include <components/camera.hpp>
#include <components/react.hpp>
#include <components/circle.hpp>

const char *WINDOW_NAME = "atls - engine";
const int WINDOW_WIDTH = 956;
const int WINDOW_HEIGHT = 540;

SDL_Window *window = nullptr;
SDL_GLContext gl_context = nullptr;
bool running;

int main()
{
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS))
        return -1;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    window = SDL_CreateWindow(
        WINDOW_NAME,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    gl_context = SDL_GL_CreateContext(window);

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
        return -1;

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    running = true;

    Scene scene(window);
    scene.Initialize(WINDOW_WIDTH, WINDOW_HEIGHT, "SampleScene");
  
    auto camera = scene.Create("MainCamera");
    camera->AddComponent<Camera>();

    // auto square =  scene.Create("Square");
    // square->AddComponent<React>(vec2(100, 100));
    // if (auto transform = square->GetComponent<Transform>()) {
    //     transform->position = {WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f, 0.0f};
    // }
    
    auto circle = scene.Create("Circle");
    circle->AddComponent<Circle>(50.0f);
    if (auto transform = circle->GetComponent<Transform>()) {
        transform->position = {WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f, 0.0f};
    }
    
    scene.Start();

    Uint64 lastCounter = SDL_GetPerformanceCounter();
    double frequency = (double)SDL_GetPerformanceFrequency();
    float deltaTime = 0.0f;

    while (running)
    {
        Uint64 currentCounter = SDL_GetPerformanceCounter();
        deltaTime = (float)((currentCounter - lastCounter) / frequency);
        lastCounter = currentCounter;

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
                running = false;
        }
        deltaTime = std::min(deltaTime, 0.05f);
        scene.Update(deltaTime);

        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DestroyContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
