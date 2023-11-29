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
    ~TextureMaterial();
    void UpdateUniforms(Scene* scene);
    void Use();


    static Shader* fragmentShader, *vertexShader, *geometryShader;
    unsigned int texture;
    ShaderProgram* program;
    Scene* scene;
};
