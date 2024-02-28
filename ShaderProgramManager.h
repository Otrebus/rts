#pragma once
#define NOMINMAX
#include "Shader.h"
#include "ShaderProgram.h"
#include "ShaderProgramManager.h"
#include <map>


class ShaderProgramManager
{
    unsigned int id;

public:
    ShaderProgramManager();
    ~ShaderProgramManager();

    ShaderProgram* getProgram(Shader* fragmentShader, Shader* geometryShader, Shader* vertexShader);
    ShaderProgram* getProgram(Shader* fragmentShader, Shader* vertexShader);

    unsigned int getId();
    void use();

    // Unordered map doesn't seem any faster for now
    std::map<std::tuple<int, int, int>, ShaderProgram*> programMap;
};
