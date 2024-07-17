#pragma once

#include "Material.h"
#include "Vector3.h"

class Shader;

class TerrainMaterial : public Material
{
public:
    TerrainMaterial();
    virtual ~TerrainMaterial();

    Shader* getShader();
    void updateUniforms(Scene* scene);
    void use();

    static Shader* fragmentShader;

    Material* clone();

    bool isFlat;
    void setFlat(bool flatness);
};
