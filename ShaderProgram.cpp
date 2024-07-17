#include "ShaderProgram.h"


ShaderProgram::ShaderProgram()
{
    id = glCreateProgram();
}

ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(id);
}

void ShaderProgram::use()
{
    glUseProgram(id);
}

unsigned int ShaderProgram::getId()
{
    return id;
}
