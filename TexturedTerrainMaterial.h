#pragma once
#pragma once

#include "Material.h"
#include "Vector3.h"
#include "ShaderProgram.h"
#include "Scene.h"

class Shader;

class TexturedTerrainMaterial : public Material
{
public:
    TexturedTerrainMaterial();
    virtual ~TexturedTerrainMaterial();

    Shader* getShader();
    void updateUniforms(Scene* scene);
    void use();

    static Shader* fragmentShader;

    bool isFlat;
    void setFlat(bool flatness);

    unsigned int texture;
    Scene* scene;
};
