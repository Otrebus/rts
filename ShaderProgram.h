#pragma once
#include "Shader.h"

class ShaderProgram
{
    unsigned int id;

public:
    ShaderProgram();
    ~ShaderProgram();

    template<typename... Shaders>
    void AddShaders(const Shaders&... shaders)
    {
        (glAttachShader(id, shaders.GetId()), ...);
        glLinkProgram(id);
    }

    unsigned int GetId();
    void Use();
};
