#pragma once
#include <SDL3/SDL.h>
#include <glad/glad.h>
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"

#include <ecs/ecs.hpp>
#include <engine/system.hpp>

class Engine
{
public:
    ~Engine() { Clean(); }
    void Initialize(const char *name, const int &w, const int &h);
    void Run();
    void Clean();

    Entity* Create(const char* name);

private:
    bool running = true;
    float deltaTime = 0.0f;

    SDL_Window *window;
    SDL_GLContext gl_context;
    EngineSystem system;
    EntityMap entities;
};