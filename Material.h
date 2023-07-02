#pragma once

#include "Shader.h"

class Material
{
public:
    virtual void SetUniforms(unsigned int program) = 0;
    virtual Shader* GetShader() = 0;
};
