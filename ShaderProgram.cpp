#include "Shader.h"
#include "ShaderProgram.h"


ShaderProgram::ShaderProgram()
{
    id = glCreateProgram();
}

void ShaderProgram::Use()
{
    glUseProgram(id);
}

unsigned int ShaderProgram::GetId()
{
    return id;
}