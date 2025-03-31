#include "LineModelMaterial.h"
#include "Scene.h"
#include "Shader.h"
#include "Vector3.h"


LineModelMaterial::LineModelMaterial(Vector3 Kd) : Kd(Kd)
{
    if(!fragmentShader)
        fragmentShader = new Shader("lineModel.frag", GL_FRAGMENT_SHADER);
}


LineModelMaterial::~LineModelMaterial()
{
}


Shader* LineModelMaterial::getShader()
{
    return fragmentShader;
}

void LineModelMaterial::updateUniforms(Scene* scene)
{
    auto program = scene->getShaderProgram();
    GLuint kdLocation = glGetUniformLocation(program->getId(), "Kd");
    glUniform3f(kdLocation, Kd.x, Kd.y, Kd.z);
}


Material* LineModelMaterial::clone()
{
    return new LineModelMaterial(*this);
}


Shader* LineModelMaterial::fragmentShader = nullptr;
    