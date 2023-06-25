#pragma once

#include "Material.h"
#include "Vector3.h"
#include "ShaderProgram.h"

class Shader;

class LambertianMaterial : public Material
{
public:
    LambertianMaterial(Vector3 Kd);
    Shader* GetShader();
    void SetUniforms(unsigned int program);

    Vector3 Kd;
    static Shader* shader;
    static GLint kdLoc;

    ShaderProgram* program;
};
