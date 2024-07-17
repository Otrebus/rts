#pragma once

#include "Material.h"
#include "Vector3.h"

class Shader;

class LineMaterial : public Material
{
public:
    LineMaterial(Vector3 Kd = Vector3(0.0, 0.0, 0.0));
    virtual ~LineMaterial();

    void updateUniforms(Scene* scene);
    void use();

    Shader* getShader();

    Material* clone();

    Vector3 Kd;
    static Shader* fragmentShader;
    static GLint kdLoc;
};
