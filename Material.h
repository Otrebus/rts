#pragma once

#include "Shader.h"
#include "Scene.h"

class Shader;

class Material
{
public:
    virtual ~Material() {};

public:
    virtual void updateUniforms(Scene* scene) = 0;
    virtual Shader* getShader() = 0;
};
