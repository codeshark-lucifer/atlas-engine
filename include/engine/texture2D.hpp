#pragma once
#include <stb/stb_image.h>
#include <string>
#include <stdexcept>
#include <glad/glad.h>

enum struct Type
{
    DIFFUSE,
    AMBIENT,
    SPECULAR
};

class Texture2D
{
public:
    unsigned int ID;
    int width, height, channels;
    bool gamma;
    bool pixelArt;
    Type type;
    std::string path;

    Texture2D(const std::string &path, const Type &type = Type::DIFFUSE, bool gamma = true, bool pixelArt = false)
    {
        this->type = type;
        this->path = path;
        this->gamma = gamma;
        this->pixelArt = pixelArt;

        LoadFromFile(path);
    }

    ~Texture2D()
    {
        glDeleteTextures(1, &ID);
    }

    void Bind(unsigned int unit = 0) const
    {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, ID);
    }

    /// <summary>
    /// Set pixel art mode for crisp, sharp edges (GL_NEAREST filter)
    /// vs smooth, blurred edges (GL_LINEAR filter)
    /// </summary>
    /// <param name="enable">true for pixel art (crisp), false for smooth rendering</param>
    void SetPixelArtMode(bool enable)
    {
        if (pixelArt == enable)
            return; // Already in desired mode

        pixelArt = enable;
        Bind(0);

        if (pixelArt)
        {
            // Pixel-perfect rendering with sharp edges
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        }
        else
        {
            // Smooth rendering with linear filtering
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
    }

private:
    void LoadFromFile(const std::string &path)
    {
        stbi_set_flip_vertically_on_load(true);

        unsigned char *data = stbi_load(
            path.c_str(),
            &width,
            &height,
            &channels,
            0);

        if (!data)
        {
            throw std::runtime_error("Failed to load texture: " + path);
        }

        GLenum internalFormat = GL_RGB;
        GLenum dataFormat = GL_RGB;

        if (channels == 1)
        {
            internalFormat = dataFormat = GL_RED;
        }
        else if (channels == 3)
        {
            dataFormat = GL_RGB;
            internalFormat = gamma ? GL_SRGB : GL_RGB;
        }
        else if (channels == 4)
        {
            dataFormat = GL_RGBA;
            internalFormat = gamma ? GL_SRGB_ALPHA : GL_RGBA;
        }

        glGenTextures(1, &ID);
        glBindTexture(GL_TEXTURE_2D, ID);

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            internalFormat,
            width,
            height,
            0,
            dataFormat,
            GL_UNSIGNED_BYTE,
            data);
        
        // Don't generate mipmaps for pixel art as it causes blurring
        if (!pixelArt)
        {
            glGenerateMipmap(GL_TEXTURE_2D);
        }

        // Texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // Use GL_NEAREST for pixel-perfect rendering, GL_LINEAR for smooth rendering
        if (pixelArt)
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        }
        else
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }

        stbi_image_free(data);
    }
};