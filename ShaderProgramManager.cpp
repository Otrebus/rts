#include "ShaderProgramManager.h"

ShaderProgramManager::ShaderProgramManager()
{
}

ShaderProgramManager::~ShaderProgramManager()
{
}

ShaderProgram* ShaderProgramManager::getProgram(Shader* fragmentShader, Shader* geometryShader, Shader* vertexShader)
{
    // This can be made faster if we just assign an id to each shader or something
    auto str = fragmentShader->getFileName() + geometryShader->getFileName() + vertexShader->getFileName();

    if(auto it = programMap.find(str); it == programMap.end())
    {
        auto shaderProgram = new ShaderProgram();
        shaderProgram->addShaders(*fragmentShader, *geometryShader, *vertexShader);
        programMap[str] = shaderProgram;
        return shaderProgram;
    }
    else
        return it->second;
}
