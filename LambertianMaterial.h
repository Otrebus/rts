#pragma once

#include "Material.h"
#include "Scene.h"
#include "ShaderProgram.h"
#include "Vector3.h"

class Shader;

class LambertianMaterial : public Material
{
public:
    LambertianMaterial(Vector3 Kd = Vector3(0.0, 0.0, 0.0));
    virtual ~LambertianMaterial();

    void updateUniforms(Scene* scene);
    void use();

    Shader* getShader();

    Material* clone();

    Vector3 Kd;
    static Shader* fragmentShader;
    static GLint kdLoc;
};
