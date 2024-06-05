#include "SelectionDecalMaterial.h"
#include "Vector3.h"
#include "Shader.h"
#include "Scene.h"


SelectionDecalMaterial::SelectionDecalMaterial(Vector3 Kd) : Kd(Kd)
{     
    if(!fragmentShader)
        fragmentShader = new Shader("selectionMaterial.frag", GL_FRAGMENT_SHADER);
}


SelectionDecalMaterial::~SelectionDecalMaterial()
{
}


Shader* SelectionDecalMaterial::getShader()
{
    return fragmentShader;
}

void SelectionDecalMaterial::updateUniforms(Scene* scene)
{
    auto program = scene->getShaderProgram();
    glUniform1f(glGetUniformLocation(program->getId(), "radius"), radius);
    GLuint kdLocation = glGetUniformLocation(program->getId(), "Kd");
    glUniform3f(kdLocation, Kd.x, Kd.y, Kd.z);

    GLuint alphaLocation = glGetUniformLocation(program->getId(), "alpha");
    glUniform1f(alphaLocation, alpha);
}


Shader* SelectionDecalMaterial::fragmentShader = nullptr;
real SelectionDecalMaterial::radius = 0;
real SelectionDecalMaterial::alpha = 0;
