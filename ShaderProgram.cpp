#include "Shader.h"
#include "ShaderProgram.h"


ShaderProgram::ShaderProgram()
{
    id = glCreateProgram();
}

ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(id);
}

void ShaderProgram::Use()
{
    glUseProgram(id);
}

unsigned int ShaderProgram::GetId()
{
    return id;
}
