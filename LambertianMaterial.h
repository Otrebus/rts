#pragma once

#include "Material.h"
#include "Vector3.h"
#include "ShaderProgram.h"
#include "Scene.h"

class Shader;

class LambertianMaterial : public Material
{
public:
    LambertianMaterial(Vector3 Kd = Vector3(0.0, 0.0, 0.0));
    ~LambertianMaterial();

    void updateUniforms(Scene* scene);
    void use();

    Shader* getShader();

    Vector3 Kd;
    static Shader* fragmentShader;
    static GLint kdLoc;
};
