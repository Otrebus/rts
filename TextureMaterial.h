#pragma once

#include "Material.h"
#include "Vector3.h"
#include "ShaderProgram.h"
#include "Scene.h"

class Shader;

class TextureMaterial : public Material
{
public:
    TextureMaterial(const std::string& textureFile);
    virtual ~TextureMaterial();

    void updateUniforms(Scene* scene);
    void use();

    Shader* getShader();

    static Shader* fragmentShader;
    unsigned int texture;
    Scene* scene;
};
