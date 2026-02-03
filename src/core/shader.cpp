#include <engine/shader.hpp>
#include <fstream>
#include <filesystem>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <utils/vector.h>
#include <utils/quat.h>
#include <utils/matrix.h>
#include <engine/color.h>

Shader::Shader(const std::string &path) : filepath(path)
{
    if (!std::filesystem::exists(path))
        throw std::runtime_error("Failed to read shader from: " + path);

    std::string vertexCode;
    std::string fragmentCode;

    std::ifstream file(path);
    std::string line;
    enum class ShaderType
    {
        NONE,
        VERTEX,
        FRAGMENT
    };
    ShaderType type = ShaderType::NONE;

    while (std::getline(file, line))
    {
        if (line.find("#shader vertex") != std::string::npos)
        {
            type = ShaderType::VERTEX;
            continue;
        }
        else if (line.find("#shader fragment") != std::string::npos)
        {
            type = ShaderType::FRAGMENT;
            continue;
        }

        // Append line to the correct shader source
        if (type == ShaderType::VERTEX)
            vertexCode += line + "\n";
        else if (type == ShaderType::FRAGMENT)
            fragmentCode += line + "\n";
    }

    // Debug output
    // std::cout << "VERTEX  :\n"
    //           << vertexCode << "\n";
    // std::cout << "FRAGMENT:\n"
    //           << fragmentCode << "\n";

    // Compile shaders
    vertexShader = Compile(vertexCode.c_str(), GL_VERTEX_SHADER);
    fragmentShader = Compile(fragmentCode.c_str(), GL_FRAGMENT_SHADER);

    // Link program
    ID = glCreateProgram();
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    glLinkProgram(ID);

    int success;
    char infoLog[512];
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        throw std::runtime_error("Failed to link shader file (" + filepath + "):\n" + std::string(infoLog));
    }
}

std::string Shader::ReadFile(const std::string &path) const
{
    if (!std::filesystem::exists(path))
        throw std::runtime_error("Failed to read shader from: " + path);

    std::ifstream file(path);
    std::string content;
    std::string line;

    while (std::getline(file, line))
    {
        content += line + "\n";
    }
    return content;
}

Shader::Shader(const std::string &vert, const std::string &frag)
{
    vertexShader = Compile(ReadFile(vert).c_str(), GL_VERTEX_SHADER);
    fragmentShader = Compile(ReadFile(frag).c_str(), GL_FRAGMENT_SHADER);

    // Link program
    ID = glCreateProgram();
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    glLinkProgram(ID);

    int success;
    char infoLog[512];
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        throw std::runtime_error("Failed to link shader file (" + filepath + "):\n" + std::string(infoLog));
    }
}

Shader::Shader(const std::string &vert, const std::string &frag, const std::string &geom)
{
    vertexShader = Compile(ReadFile(vert).c_str(), GL_VERTEX_SHADER);
    fragmentShader = Compile(ReadFile(frag).c_str(), GL_FRAGMENT_SHADER);
    geometryShader = Compile(ReadFile(geom).c_str(), GL_GEOMETRY_SHADER);

    // Link program
    ID = glCreateProgram();
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    glAttachShader(ID, geometryShader);
    glLinkProgram(ID);

    int success;
    char infoLog[512];
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        throw std::runtime_error("Failed to link shader file (" + filepath + "):\n" + std::string(infoLog));
    }
}

Shader::~Shader()
{
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glDeleteShader(geometryShader);

    glDeleteProgram(ID);
}

void Shader::Use() const
{
    glUseProgram(ID);
}

unsigned int Shader::Compile(const char *source, GLenum type = GL_VERTEX_SHADER)
{
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    std::string mode = "vertex";
    if(type == GL_FRAGMENT_SHADER)
        mode = "fragment";
    else if(type == GL_GEOMETRY_SHADER)
        mode = "geometry";

    if (!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        throw std::runtime_error("Failed to compile " + mode + " of shader file:(" + filepath + "):\n" + std::string(infoLog));
    }
    return shader;
}

// set uniform for
template <>
void Shader::SetUniform<float>(const std::string &name, float value) const
{
    GLint loc = glGetUniformLocation(ID, name.c_str());
    glUniform1f(loc, value);
}

template <>
void Shader::SetUniform<int>(const std::string &name, int value) const
{
    GLint loc = glGetUniformLocation(ID, name.c_str());
    glUniform1i(loc, value);
}

template <>
void Shader::SetUniform<bool>(const std::string &name, bool value) const
{
    GLint loc = glGetUniformLocation(ID, name.c_str());
    glUniform1i(loc, static_cast<int>(value));
}

template <>
void Shader::SetUniform<double>(const std::string &name, double value) const
{
    GLint loc = glGetUniformLocation(ID, name.c_str());
    glUniform1f(loc, static_cast<float>(value));
}

template <>
void Shader::SetUniform<vec2>(const std::string &name, vec2 value) const
{
    GLint loc = glGetUniformLocation(ID, name.c_str());
    glUniform2f(loc, value.x, value.y);
}

template <>
void Shader::SetUniform<vec3>(const std::string &name, vec3 value) const
{
    GLint loc = glGetUniformLocation(ID, name.c_str());
    glUniform3f(loc, value.x, value.y, value.z);
}

template <>
void Shader::SetUniform<vec4>(const std::string &name, vec4 value) const
{
    GLint loc = glGetUniformLocation(ID, name.c_str());
    glUniform4f(loc, value.x, value.y, value.z, value.w);
}

template <>
void Shader::SetUniform<Color>(const std::string &name, Color value) const
{
    GLint loc = glGetUniformLocation(ID, name.c_str());
    glUniform4f(loc, value.r, value.g, value.b, value.a);
}

template <>
void Shader::SetUniform<mat4>(const std::string &name, mat4 value) const
{
    GLint loc = glGetUniformLocation(ID, name.c_str());
    glUniformMatrix4fv(loc, 1, GL_FALSE, value.m);
}

// vec2, vec3, vec4, mat3, mat4
