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

        // Mouse position & buttons
        Uint32 state = SDL_GetMouseState(&mouseX, &mouseY);
        mouseDX = mouseX - prevMouseX;
        mouseDY = mouseY - prevMouseY;

        // Mouse buttons (SDL3 uses SDL_BUTTON_LEFT / SDL_BUTTON_RIGHT etc.)
        bool downLeft = state & SDL_BUTTON_LEFT;
        bool downRight = state & SDL_BUTTON_RIGHT;
        bool downMiddle = state & SDL_BUTTON_MIDDLE;

        UpdateMouseButton(1, downLeft);
        UpdateMouseButton(2, downRight);
        UpdateMouseButton(3, downMiddle);

        // Keyboard (SDL3 returns const bool*)
        const bool *keys = SDL_GetKeyboardState(nullptr);
        for (int i = 0; i < SDL_SCANCODE_COUNT; i++)
        {
            keyDown[i] = keys[i];
            keyPressed[i] = keys[i] && !keyPrev[i];
            keyReleased[i] = !keys[i] && keyPrev[i];
            keyPrev[i] = keys[i];
        }
    }

    void HandleEvent(const SDL_Event &e)
    {
        if (e.type == SDL_EVENT_MOUSE_WHEEL)
        {
            mouseWheelX = e.wheel.x;
            mouseWheelY = e.wheel.y;
        }
    }
    vec2 GetMoveDirection() const
    {
        vec2 dir{0.0f, 0.0f};

        if (IsKeyDown(SDL_SCANCODE_A))
            dir.x -= 1.0f;
        if (IsKeyDown(SDL_SCANCODE_D))
            dir.x += 1.0f;
        if (IsKeyDown(SDL_SCANCODE_W))
            dir.y += 1.0f;
        if (IsKeyDown(SDL_SCANCODE_S))
            dir.y -= 1.0f;

        float len = sqrt(dir.x * dir.x + dir.y * dir.y);
        if (len > 0.0f)
        {
            dir.x /= len;
            dir.y /= len;
        }

        return dir;
    }

    // --- Mouse ---
    bool IsMouseButtonDown(int button) const { return mouseButtonDown[button]; }
    bool IsMouseButtonPressed(int button) const { return mouseButtonPressed[button]; }
    bool IsMouseButtonReleased(int button) const { return mouseButtonReleased[button]; }

    float GetMouseX() const { return mouseX; }
    float GetMouseY() const { return mouseY; }
    float GetMouseDeltaX() const { return mouseDX; }
    float GetMouseDeltaY() const { return mouseDY; }
    float GetMouseWheelX() const { return mouseWheelX; }
    float GetMouseWheelY() const { return mouseWheelY; }

    void SetMouseVisible(bool visible)
    {
        if (visible)
            SDL_ShowCursor();
        else
            SDL_HideCursor();
    }

    void LockMouse(bool lock)
    {
        SDL_SetWindowRelativeMouseMode(window, lock);
    }

    bool IsMouseLocked() const
    {
        return SDL_GetWindowRelativeMouseMode(window);
    }

    // --- Keyboard ---
    bool IsKeyDown(int scancode) const { return keyDown[scancode]; }
    bool IsKeyPressed(int scancode) const { return keyPressed[scancode]; }
    bool IsKeyReleased(int scancode) const { return keyReleased[scancode]; }

private:
    InputManager() = default;

    void UpdateMouseButton(int button, bool down)
    {
        mouseButtonDown[button] = down;
        mouseButtonPressed[button] = down && !mouseButtonPrev[button];
        mouseButtonReleased[button] = !down && mouseButtonPrev[button];
        mouseButtonPrev[button] = down;
    }

    SDL_Window *window = nullptr;

    // Mouse
    float mouseX = 0.0f;
    float mouseY = 0.0f;

    int prevMouseX = 0;
    int prevMouseY = 0;
    int mouseDX = 0;
    int mouseDY = 0;
    float mouseWheelX = 0.0f;
    float mouseWheelY = 0.0f;

    bool mouseButtonDown[6] = {false};
    bool mouseButtonPressed[6] = {false};
    bool mouseButtonReleased[6] = {false};
    bool mouseButtonPrev[6] = {false};

    // Keyboard
    bool keyDown[SDL_SCANCODE_COUNT] = {false};
    bool keyPressed[SDL_SCANCODE_COUNT] = {false};
    bool keyReleased[SDL_SCANCODE_COUNT] = {false};
    bool keyPrev[SDL_SCANCODE_COUNT] = {false};
};
