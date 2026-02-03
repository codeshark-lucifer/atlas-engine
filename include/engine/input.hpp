#pragma once
#include <engine/singleton.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_mouse.h>

class InputManager : public Singleton<InputManager>
{
    friend class Singleton<InputManager>;

public:
    void Initialize(SDL_Window *window)
    {
        this->window = window;
    }

    void Update()
    {
        prevMouseX = mouseX;
        prevMouseY = mouseY;

        mouseWheelX = 0.0f;
        mouseWheelY = 0.0f;

        mouseButtons = SDL_GetMouseState(&mouseX, &mouseY);
        SDL_GetRelativeMouseState(&mouseDX, &mouseDY);
    }

    float GetMouseX() const { return mouseX; }
    float GetMouseY() const { return mouseY; }

    float GetMouseDeltaX() const { return mouseDX; }
    float GetMouseDeltaY() const { return mouseDY; }

    bool IsKeyPressed(SDL_Scancode key) const
    {
        const bool *keys = SDL_GetKeyboardState(nullptr);
        return keys[key];
    }

    bool IsMouseButtonPressed(Uint32 button) const
    {
        return mouseButtons == button;
    }

    void SetMouseVisible(bool visible)
    {
        bool success = visible ? SDL_ShowCursor() : SDL_HideCursor();
    }

    void LockMouse(bool lock)
    {
        SDL_SetWindowRelativeMouseMode(window, lock);
    }

    bool IsMouseLocked() const
    {
        return SDL_GetWindowRelativeMouseMode(window);
    }

    void HandleEvent(const SDL_Event &e)
    {
        switch (e.type)
        {
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP:
            mouseButtons = SDL_GetMouseState(nullptr, nullptr);
            break;

        case SDL_EVENT_MOUSE_WHEEL:
            mouseWheelX = e.wheel.x;
            mouseWheelY = e.wheel.y;
            break;

        default:
            break;
        }
    }

private:
    InputManager() = default;

private:
    SDL_Window *window = nullptr;
    float mouseWheelX = 0.0f;
    float mouseWheelY = 0.0f;

    float mouseX = 0.0f;
    float mouseY = 0.0f;
    float prevMouseX = 0.0f;
    float prevMouseY = 0.0f;

    float mouseDX = 0.0f;
    float mouseDY = 0.0f;

    Uint32 mouseButtons = 0;
};
