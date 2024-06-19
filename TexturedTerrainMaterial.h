#pragma once

#include "Material.h"
#include "Scene.h"
#include "ShaderProgram.h"
#include "Vector3.h"

class Shader;

class TexturedTerrainMaterial : public Material
{
public:
    TexturedTerrainMaterial();
    virtual ~TexturedTerrainMaterial();

    Shader* getShader();
    void updateUniforms(Scene* scene);
    void use();
    Material* clone();

    static Shader* fragmentShader;

    unsigned int texture;
    unsigned int texture2;

    unsigned int splatMap;
    Scene* scene;
};
