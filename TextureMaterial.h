#pragma once

#include "Material.h"
#include "Vector3.h"
#include "ShaderProgram.h"

class Shader;

class TextureMaterial : public Material
{
public:
    TextureMaterial(const std::string& textureFile);
    Shader* GetShader();
    void SetUniforms(unsigned int program);

    static Shader* shader;
    unsigned int texture;
    ShaderProgram* program;
};
