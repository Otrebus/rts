#pragma once

#include "Scene.h"
#include "Shader.h"

class Shader;

class Material
{
public:
    virtual ~Material() {};
    virtual Material* clone() = 0;

public:
    virtual void updateUniforms(Scene* scene) = 0;
    virtual Shader* getShader() = 0;
};
