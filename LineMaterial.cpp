#include "LineMaterial.h"
#include "Vector3.h"
#include "Shader.h"
#include "Scene.h"


LineMaterial::LineMaterial(Vector3 Kd) : Kd(Kd)
{     
    if(!fragmentShader)
        fragmentShader = new Shader("line.frag", GL_FRAGMENT_SHADER);
}


LineMaterial::~LineMaterial()
{
}


Shader* LineMaterial::getShader()
{
    return fragmentShader;
}

void LineMaterial::updateUniforms(Scene* scene)
{
    auto program = scene->getShaderProgram();
    glUniform3fv(glGetUniformLocation(program->getId(), "camPos"), 1, (GLfloat*) &scene->getCamera()->getPos());
    GLuint kdLocation = glGetUniformLocation(program->getId(), "Kd");
    glUniform3f(kdLocation, Kd.x, Kd.y, Kd.z);
}


Material* LineMaterial::clone()
{
    return new LineMaterial(*this);
}


Shader* LineMaterial::fragmentShader = nullptr;
