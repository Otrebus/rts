#pragma once

#include "Material.h"
#include "Scene.h"
#include "ShaderProgram.h"
#include "Vector3.h"

class Shader;

class TextureMaterial : public Material
{
public:
    TextureMaterial(const std::string& textureFile);
    virtual ~TextureMaterial();

    void updateUniforms(Scene* scene);
    void use();

    Material* clone();

    Shader* getShader();

    static Shader* fragmentShader;
    unsigned int texture;
    Scene* scene;
};
