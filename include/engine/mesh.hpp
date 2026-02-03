#pragma once
#include <utils/vector.h>
#include <vector>
#include <buffers/vao.h>

typedef struct Vertex
{
    vec2 position;
    vec2 uv;
} Vertex;

class Mesh
{
public:
    Mesh(
        const std::vector<Vertex> &vertices,
        const std::vector<unsigned int> &indices)
        : vertices(vertices), indices(indices)
    {
        vao.Bind();
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, position));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, uv));
        glEnableVertexAttribArray(1);
        vao.Unbind();
    }

    ~Mesh()
    {
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
    }

    void Draw()
    {
        vao.Bind();
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
        vao.Unbind();
    }

    
    std::vector<vec2> GetPoints()
    {
        std::vector<vec2> points;
        points.reserve(vertices.size());

        for (const auto &v : vertices)
        {
            points.push_back(v.position);
        }

        return points;
    }

    std::vector<uint32_t> GetIndices()
    {
        return indices;
    }

private:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    VAO vao;
    unsigned int vbo = 0, ebo = 0;
};