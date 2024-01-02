#pragma once

#include "Shader.h"
#include "Scene.h"

class Material
{
public:
    virtual void use() = 0;
    virtual ~Material() {};
public:
    virtual void updateUniforms(Scene* scene) = 0;
};
