#pragma once
#define NOMINMAX
#include "Shader.h"

class ShaderProgram
{
    unsigned int id;

public:
    ShaderProgram();
    ~ShaderProgram();

    template<typename... Shaders>
    void addShaders(const Shaders&... shaders)
    {
        (glAttachShader(id, shaders.getId()), ...);
        glLinkProgram(id);

        GLint success;
        GLchar infoLog[1024];
        glGetProgramiv(id, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(id, 1024, nullptr, infoLog);
            std::cerr << "Program Linking Error: " << infoLog << std::endl;
        }
    }

    unsigned int getId();
    void use();
};
 