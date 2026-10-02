#pragma once

#include <glad/gl.h>
#include <string>

class Texture
{
public:
    GLuint ID = 0;
    int width = 0;
    int height = 0;
    int channels = 0;

    Texture() = default;
    explicit Texture(const std::string &filePath);
    ~Texture();

    void bind(GLuint unit = 0) const;
    void unbind() const;
    bool isValid() const { return ID != 0; }
};
