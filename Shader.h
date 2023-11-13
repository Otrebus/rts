#pragma once

#include <iostream>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

class Shader
{
    unsigned int id;

public:
    Shader(std::string filename, unsigned int type);
    ~Shader();
    unsigned int GetId() const;
};
