#include "TerrainMaterial.h"
#include "Vector3.h"
#include "Shader.h"
#include "Scene.h"


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
    glUniform3fv(glGetUniformLocation(scene->getShaderProgram()->getId(), "camPos"), 1, (GLfloat*) &scene->getCamera()->getPos());
}


Shader* TerrainMaterial::fragmentShader = nullptr;
