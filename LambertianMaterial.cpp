#include "LambertianMaterial.h"
#include "Vector3.h"
#include "Shader.h"
#include "Scene.h"


LambertianMaterial::LambertianMaterial(Vector3 Kd) : Kd(Kd)
{     
    if(!fragmentShader) // Or rather !initialized or something
        fragmentShader = new Shader("lambertian.frag", GL_FRAGMENT_SHADER);
}


LambertianMaterial::~LambertianMaterial()
{
}


Shader* LambertianMaterial::getShader()
{
    return fragmentShader;
}

void LambertianMaterial::updateUniforms(Scene* scene)
{
    auto program = scene->getShaderProgram();
    glUniform3fv(glGetUniformLocation(program->getId(), "camPos"), 1, (GLfloat*) &scene->getCamera()->pos);
    GLuint kdLocation = glGetUniformLocation(program->getId(), "Kd");
    glUniform3f(kdLocation, Kd.x, Kd.y, Kd.z);
}


Shader* LambertianMaterial::fragmentShader = nullptr;
