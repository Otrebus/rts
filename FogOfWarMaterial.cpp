#include "FogOfWarMaterial.h"
#include "Scene.h"
#include "Shader.h"
#include "Vector3.h"


FogOfWarMaterial::FogOfWarMaterial(Vector3 Kd) : Kd(Kd)
{
    if(!fragmentShader)
        fragmentShader = new Shader("fowShader.frag", GL_FRAGMENT_SHADER);
}


FogOfWarMaterial::~FogOfWarMaterial()
{
}


Shader* FogOfWarMaterial::getShader()
{
    return fragmentShader;
}

void FogOfWarMaterial::updateUniforms(Scene* scene)
{
    auto program = scene->getShaderProgram();
    glUniform3fv(glGetUniformLocation(program->getId(), "camPos"), 1, (GLfloat*)&scene->getCamera()->getPos());
    GLuint kdLocation = glGetUniformLocation(program->getId(), "Kd");
    glUniform3f(kdLocation, Kd.x, Kd.y, Kd.z);
}


Material* FogOfWarMaterial::clone()
{
    return new FogOfWarMaterial(*this);
}


Shader* FogOfWarMaterial::fragmentShader = nullptr;
