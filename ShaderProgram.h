#pragma once
#define NOMINMAX
#include "Shader.h"

class ShaderProgram
{
    unsigned int id;

public:
    ShaderProgram();
    ~ShaderProgram();

    template<typename... Shaders>
    void addShaders(const Shaders&... shaders)
    {
        (glAttachShader(id, shaders.getId()), ...);
        glLinkProgram(id);
    }

    unsigned int getId();
    void use();
};
