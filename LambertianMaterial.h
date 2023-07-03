#pragma once

#include "Material.h"
#include "Vector3.h"
#include "ShaderProgram.h"

class Shader;

class LambertianMaterial : public Material
{
public:
    LambertianMaterial(Vector3 Kd = Vector3(0.0, 0.0, 0.0));
    Shader* GetShader();
    void SetUniforms(unsigned int program);

    Vector3 Kd;
    static Shader* shader;
    static GLint kdLoc;

    ShaderProgram* program;
};
