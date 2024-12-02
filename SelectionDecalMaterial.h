#pragma once

#include "Material.h"
#include "Vector3.h"

class Shader;

class SelectionDecalMaterial : public Material
{
public:
    SelectionDecalMaterial(Vector3 Kd = Vector3(0.0, 0.0, 0.0), int length = 3, int width = 3, bool circular = true);
    virtual ~SelectionDecalMaterial();

    void updateUniforms(Scene* scene);
    void use();

    Shader* getShader();

    Material* clone();

    Vector3 Kd;
    static Shader* fragmentShader;
    static GLint kdLoc;

    static real radius;
    static real alpha;

    int length, width;
    bool circular;
};
