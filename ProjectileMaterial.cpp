#include "ProjectileMaterial.h"
#include "Scene.h"
#include "Shader.h"
#include "Vector3.h"


ProjectileMaterial::ProjectileMaterial(Vector3 Kd) : Kd(Kd)
{
    if(!fragmentShader)
        fragmentShader = new Shader("projectileMaterial.frag", GL_FRAGMENT_SHADER);
}


ProjectileMaterial::~ProjectileMaterial()
{
}


Shader* ProjectileMaterial::getShader()
{
    return fragmentShader;
}

void ProjectileMaterial::updateUniforms(Scene* scene)
{
    auto program = scene->getShaderProgram();
    glUniform3fv(glGetUniformLocation(program->getId(), "camPos"), 1, (GLfloat*)&scene->getCamera()->getPos());
    GLuint kdLocation = glGetUniformLocation(program->getId(), "Kd");
    glUniform3f(kdLocation, Kd.x, Kd.y, Kd.z);
}

Material* ProjectileMaterial::clone()
{
    return new ProjectileMaterial(*this);
}

Shader* ProjectileMaterial::fragmentShader = nullptr;
