#pragma once
#include "Shader.h"
#include "ShaderProgram.h"
#include "ShaderProgramManager.h"
#include <unordered_map>

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

    std::unordered_map<std::string, ShaderProgram*> programMap;
};
