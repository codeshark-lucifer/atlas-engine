#include <engine/engine.hpp>

#include <vector>
#include <string>
#include <thread>
#include <atomic>
#include <algorithm>
#include <stdexcept>

void Engine::Initialize(const char *name, const int &w, const int &h)
{
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
        throw std::runtime_error("Failed to initialize SDL.");

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

    window = SDL_CreateWindow(name, w, h, SDL_WindowFlags(SDL_WINDOW_OPENGL));
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplSDL3_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init("#version 460");

    system.Initialize(w, h);
}

void Engine::Run()
{
    Uint64 lastFrame = SDL_GetPerformanceCounter();

    system.Start(entities);
    while (running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL3_ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT)
                running = false;
            if (event.type == SDL_EVENT_WINDOW_RESIZED)
                system.Resize(entities, (int)event.window.data1, (int)event.window.data2);
        }

        Uint64 currentFrame = SDL_GetPerformanceCounter();
        deltaTime = (float)(currentFrame - lastFrame) / (float)SDL_GetPerformanceFrequency();
        lastFrame = currentFrame;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
        {
            // Draw UI
            system.DrawUI(entities);
        }
        ImGui::Render();
        system.Update(entities, deltaTime);
        {
            UpdateTransforms(entities);
            system.Render(entities);
        }
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }
}

void Engine::Clean()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DestroyContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

Entity *Engine::Create(const char *name)
{
    return CreateEntity(name, entities);
}

// Entity *Engine::Load(const char *path)