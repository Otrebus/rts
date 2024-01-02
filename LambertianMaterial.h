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
    Shader* getShader();
    void updateUniforms(Scene* scene);
    void use();

    Vector3 Kd;
    static Shader* fragmentShader;
    static Shader* vertexShader;
    static Shader* geometryShader;
    static GLint kdLoc;

    ShaderProgram* program;
};
