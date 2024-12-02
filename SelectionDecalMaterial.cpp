#include "SelectionDecalMaterial.h"

SelectionDecalMaterial::SelectionDecalMaterial(Vector3 Kd, int length, int width, bool circular) : Kd(Kd), length(length), width(width), circular(circular)
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
    glUniform1i(glGetUniformLocation(program->getId(), "length"), length);
    glUniform1i(glGetUniformLocation(program->getId(), "width"), width);
    glUniform1i(glGetUniformLocation(program->getId(), "circular"), int(circular));
    GLuint kdLocation = glGetUniformLocation(program->getId(), "Kd");
    glUniform3f(kdLocation, Kd.x, Kd.y, Kd.z);

    GLuint alphaLocation = glGetUniformLocation(program->getId(), "alpha");
    glUniform1f(alphaLocation, alpha);
}

Material* SelectionDecalMaterial::clone()
{
    return new SelectionDecalMaterial(*this);
}

Shader* SelectionDecalMaterial::fragmentShader = nullptr;
real SelectionDecalMaterial::radius = 0;
real SelectionDecalMaterial::alpha = 0;
