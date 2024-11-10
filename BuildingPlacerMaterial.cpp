#include "BuildingPlacerMaterial.h"


BuildingPlacerMaterial::BuildingPlacerMaterial(Vector3 Kd) : Kd(Kd)
{
    if(!fragmentShader)
        fragmentShader = new Shader("selectionMaterial.frag", GL_FRAGMENT_SHADER);
}


BuildingPlacerMaterial::~BuildingPlacerMaterial()
{
}


Shader* BuildingPlacerMaterial::getShader()
{
    return fragmentShader;
}

void BuildingPlacerMaterial::updateUniforms(Scene* scene)
{
    auto program = scene->getShaderProgram();
    GLuint kdLocation = glGetUniformLocation(program->getId(), "Kd");
    glUniform3f(kdLocation, Kd.x, Kd.y, Kd.z);
}

Material* BuildingPlacerMaterial::clone()
{
    return new BuildingPlacerMaterial(*this);
}


Shader* BuildingPlacerMaterial::fragmentShader = nullptr;
