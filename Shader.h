#pragma once

#include <iostream>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

class Shader
{
    unsigned int id;

public:
    Shader(std::string filename, unsigned int type);
    unsigned int GetId() const;
};

class ShaderProgram
{
    unsigned int id;

public:
    ShaderProgram();

    template<typename... Shaders>
    void AddShaders(const Shaders&... shaders)
    {
        (glAttachShader(id, shaders.GetId()), ...);
        glLinkProgram(id);
    }

    void Use();
};