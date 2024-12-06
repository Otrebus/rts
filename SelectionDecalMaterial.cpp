#include "SelectionDecalMaterial.h"

SelectionDecalMaterial::SelectionDecalMaterial(Vector3 Kd, bool circular) : Kd(Kd), circular(circular)
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
    glUniform1f(glGetUniformLocation(program->getId(), "radiusA"), radiusA);
    glUniform1f(glGetUniformLocation(program->getId(), "radiusB"), radiusB);
    glUniform1i(glGetUniformLocation(program->getId(), "circular"), int(circular));
    glUniform3f(glGetUniformLocation(program->getId(), "Kd"), Kd.x, Kd.y, Kd.z);
    glUniform1f(glGetUniformLocation(program->getId(), "alpha"), alpha);
}

Material* SelectionDecalMaterial::clone()
{
    return new SelectionDecalMaterial(*this);
}

Shader* SelectionDecalMaterial::fragmentShader = nullptr;
real SelectionDecalMaterial::radiusA = 0;
real SelectionDecalMaterial::radiusB = 0;
real SelectionDecalMaterial::alpha = 0;
