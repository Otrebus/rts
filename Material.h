#pragma once

#include "Shader.h"
#include "Scene.h"

class Material
{
public:
    virtual void Use() = 0;
public:
    virtual void UpdateUniforms(Scene* scene) = 0;
};
