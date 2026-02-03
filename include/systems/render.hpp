#pragma once
#include <ecs/system.hpp>
#include <engine/shape.h>
#include <components/camera.hpp>
#include <memory>

enum struct MSAALevel
{
    DISABLED = 0,
    MSAA_2X = 2,
    MSAA_4X = 4,
    MSAA_8X = 8,
    MSAA_16X = 16
};

class RenderSystem : public System
{
public:
    RenderSystem() : System("RenderSystem") {}
    ~RenderSystem() = default;

    void Initialize(const int &width, const int &height)
    {
        this->width = width;
        this->height = height;

        defaultShader = std::make_unique<Shader>("assets/shaders/scene.glsl");
        
        // Detect current MSAA samples if MSAA is enabled
        DetectMSAA();
    }

    void OnStart(World &world) override
    {
        for (const auto &[id, entity] : world.GetEntities())
        {
            if (auto camera = entity->GetComponent<Camera>())
            {
                defaultCamera = camera;
                break; // Assuming there is only one camera
            }
        }
    }

    void OnRender(World &world) override
    {
        glClearColor(bg_color.r, bg_color.g, bg_color.b, bg_color.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (defaultCamera)
            defaultCamera->SetUniform(*defaultShader);

        for (const auto &[id, entity] : world.GetEntities())
        {
            // Try to find any Shape component (including derived types like Circle)
            for (const auto &comp : entity->GetAllComponent())
            {
                if (auto shape = dynamic_cast<Shape*>(comp))
                {
                    shape->Render(*defaultShader);
                }
            }
        }
    }

    void OnUI(World &world) override
    {
    }

    void OnResize(World &world, int w, int h) override
    {
        glViewport(0, 0, w, h);
    }

    void SetBGColor(Color color)
    {
        this->bg_color = color;
    }

    /// <summary>
    /// Enable or disable MSAA (Multi-Sample Anti-Aliasing)
    /// </summary>
    /// <param name="enable">true to enable MSAA, false to disable</param>
    void SetMSAAEnabled(bool enable)
    {
        if (enable)
        {
            glEnable(GL_MULTISAMPLE);
        }
        else
        {
            glDisable(GL_MULTISAMPLE);
        }
        msaaEnabled = enable;
    }

    /// <summary>
    /// Get current MSAA status
    /// </summary>
    bool IsMSAAEnabled() const { return msaaEnabled; }

    /// <summary>
    /// Get current number of MSAA samples
    /// </summary>
    int GetMSAASamples() const { return msaaSamples; }

    /// <summary>
    /// Get MSAA level as enum
    /// </summary>
    MSAALevel GetMSAALevel() const
    {
        switch (msaaSamples)
        {
            case 2:  return MSAALevel::MSAA_2X;
            case 4:  return MSAALevel::MSAA_4X;
            case 8:  return MSAALevel::MSAA_8X;
            case 16: return MSAALevel::MSAA_16X;
            default: return MSAALevel::DISABLED;
        }
    }

private:
    int width = 0, height = 0;
    std::unique_ptr<Shader> defaultShader = nullptr;

    Camera *defaultCamera = nullptr;
    Color bg_color = BACKGROUND;
    
    bool msaaEnabled = true;
    int msaaSamples = 0;

    /// <summary>
    /// Detect the current MSAA sample count from OpenGL
    /// </summary>
    void DetectMSAA()
    {
        glGetIntegerv(GL_SAMPLES, &msaaSamples);
        GLint sampleBuffers = 0;
        glGetIntegerv(GL_SAMPLE_BUFFERS, &sampleBuffers);
        
        msaaEnabled = (sampleBuffers > 0) && (msaaSamples > 0);
    }

};