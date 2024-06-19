#include "Scene.h"
#include "Shader.h"
#include "TerrainMaterial.h"
#include "Vector3.h"


TerrainMaterial::TerrainMaterial()
{
    if(!fragmentShader)
        fragmentShader = new Shader("terrain.frag", GL_FRAGMENT_SHADER);
}


TerrainMaterial::~TerrainMaterial()
{
}


Shader* TerrainMaterial::getShader()
{
    return fragmentShader;
}


void TerrainMaterial::updateUniforms(Scene* scene)
{
    glUniform3fv(glGetUniformLocation(scene->getShaderProgram()->getId(), "camPos"), 1, (GLfloat*)&scene->getCamera()->getPos());
    glUniform1i(glGetUniformLocation(scene->getShaderProgram()->getId(), "flatShaded"), isFlat);
}


void TerrainMaterial::setFlat(bool flat)
{
    isFlat = flat;
}

Material* TerrainMaterial::clone()
{
    return new TerrainMaterial(*this);
}

Shader* TerrainMaterial::fragmentShader = nullptr;
