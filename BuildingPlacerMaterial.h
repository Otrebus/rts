
#pragma once

#include "Material.h"
#include "Vector3.h"

class Shader;

class BuildingPlacerMaterial : public Material
{
public:
    BuildingPlacerMaterial(Vector3 Kd = Vector3(0.0, 0.0, 0.0));
    virtual ~BuildingPlacerMaterial();

    void updateUniforms(Scene* scene);
    void use();

    Shader* getShader();

    Material* clone();

    Vector3 Kd;
    static Shader* fragmentShader;
    static GLint kdLoc;
};
