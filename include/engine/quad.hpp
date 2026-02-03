#pragma once
#include <engine/mesh.hpp>

static std::vector<Vertex> vertices = {
    {{-50.0f, -50.0f}, {0.0f, 0.0f}},
    {{50.0f, -50.0f}, {1.0f, 0.0f}},
    {{50.0f, 50.0f}, {1.0f, 1.0f}},
    {{-50.0f, 50.0f}, {0.0f, 1.0f}},
};

static std::vector<unsigned int> indices = {
    0, 1, 2,
    2, 3, 0};

class Quad
{
public:
    Quad() 
    : mesh(vertices, indices) 
    {
    
    };
    
    void Draw()
    {
        mesh.Draw();
    }

private:
    Mesh mesh;
};