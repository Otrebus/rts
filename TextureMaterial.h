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

    Vector3 Kd;
    static Shader* shader;
    static GLint kdLoc;

    ShaderProgram* program;
};
