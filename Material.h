#pragma once

#include "Shader.h"
#include "Scene.h"

class Material
{
public:
    virtual void Use() = 0;
    virtual ~Material() {};
public:
    virtual void UpdateUniforms(Scene* scene) = 0;
};
