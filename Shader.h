#pragma once
#define NOMINMAX
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <iostream>

class Shader
{
    unsigned int id;
    std::string filename;

public:
    Shader(std::string filename, unsigned int type);
    ~Shader();
    unsigned int getId() const;
    std::string getFileName();
};
