#pragma once
#include <glad/glad.h>
#include <string>

class Shader
{
public:
    Shader(const std::string &path);
    Shader(const std::string &vert, const std::string &frag);
    Shader(const std::string &vert, const std::string &frag, const std::string &geom);
    ~Shader();

    void Use() const;
    std::string ReadFile(const std::string& path) const;

    template <typename T>
    void SetUniform(const std::string &name, T value) const;

private:
    std::string filepath;
    unsigned int ID = 0;
    unsigned int vertexShader = 0;
    unsigned int fragmentShader = 0;
    unsigned int geometryShader = 0;

    unsigned int Compile(const char *source, GLenum type);
};
