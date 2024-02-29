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

    for(auto& t : programMap)
    {
        if(t.first == key)
            return t.second;
    }

    auto shaderProgram = new ShaderProgram();
    shaderProgram->addShaders(*fragmentShader, *geometryShader, *vertexShader);
    programMap.push_back({ key, shaderProgram });
    /*if(auto it = programMap.find(key); it == programMap.end())
    {
        auto shaderProgram = new ShaderProgram();
        shaderProgram->addShaders(*fragmentShader, *geometryShader, *vertexShader);
        programMap[key] = shaderProgram;
        return shaderProgram;
    }
    else
        return it->second;*/
}

ShaderProgram* ShaderProgramManager::getProgram(Shader* fragmentShader, Shader* vertexShader)
{
    auto key = std::tuple(fragmentShader->getId(), 0, vertexShader->getId());

    for(auto& t : programMap)
    {
        if(t.first == key)
            return t.second;
    }

    auto shaderProgram = new ShaderProgram();
    shaderProgram->addShaders(*fragmentShader, *vertexShader);
    programMap.push_back({ key, shaderProgram });
}