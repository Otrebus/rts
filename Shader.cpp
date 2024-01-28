#include <fstream>
#include "Shader.h"
#include <sstream>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>


Shader::Shader(std::string filename, unsigned int type)
{
    this->filename = filename;
    std::ifstream file(filename);
    
    // Check if file was opened successfully
    if (!file.is_open())
        throw std::runtime_error("Failed to open file: " + filename);

    std::stringstream buffer;
    buffer << file.rdbuf();
    
    const std::string source = buffer.str();
    auto cstr = source.c_str();

    id = glCreateShader(type);
    glShaderSource(id, 1, &cstr, NULL);
    glCompileShader(id);

    int success;
    char infoLog[512];
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);

    if(!success)
    {
        glGetShaderInfoLog(id, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
}

Shader::~Shader()
{
    glDeleteShader(id);
}

unsigned int Shader::getId() const
{
    return id;
}

std::string Shader::getFileName()
{
    return filename;
}