#include "ShaderProgramManager.h"

ShaderProgramManager::ShaderProgramManager()
{
}

ShaderProgramManager::~ShaderProgramManager()
{
}

ShaderProgram* ShaderProgramManager::getProgram(Shader* fragmentShader, Shader* geometryShader, Shader* vertexShader)
{
    auto key = std::tuple(fragmentShader->getId(), geometryShader->getId(), vertexShader->getId());

    if(auto it = programMap.find(key); it == programMap.end())
    {
        auto shaderProgram = new ShaderProgram();
        shaderProgram->addShaders(*fragmentShader, *geometryShader, *vertexShader);
        programMap[key] = shaderProgram;
        return shaderProgram;
    }
    else
        return it->second;
}

ShaderProgram* ShaderProgramManager::getProgram(Shader* fragmentShader, Shader* vertexShader)
{
    auto key = std::tuple(fragmentShader->getId(), 0, vertexShader->getId());

    if(auto it = programMap.find(key); it == programMap.end())
    {
        auto shaderProgram = new ShaderProgram();
        shaderProgram->addShaders(*fragmentShader, *vertexShader);
        programMap[key] = shaderProgram;
        return shaderProgram;
    }
    else
        return it->second;
}