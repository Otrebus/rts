#pragma once
#pragma once

#include "Material.h"
#include "Vector3.h"
#include "ShaderProgram.h"
#include "Scene.h"

class Shader;

class LineMaterial : public Material
{
public:
    LineMaterial(Vector3 Kd = Vector3(0.0, 0.0, 0.0));
    ~LineMaterial();

    void updateUniforms(Scene* scene);
    void use();

    Shader* getShader();

    Vector3 Kd;
    static Shader* fragmentShader;
    static GLint kdLoc;
};
