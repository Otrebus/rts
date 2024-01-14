#include "LambertianMaterial.h"
#include "Vector3.h"
#include "Shader.h"
#include "Scene.h"


LambertianMaterial::LambertianMaterial(Vector3 Kd) : Kd(Kd)
{     
    if(!fragmentShader) // Or rather !initialized or something
        fragmentShader = new Shader("lambertian.frag", GL_FRAGMENT_SHADER);
    if(!vertexShader)
        vertexShader = new Shader("vertexShader.vert", GL_VERTEX_SHADER);
    if(!geometryShader)
        geometryShader = new Shader("geometryShader.geom", GL_GEOMETRY_SHADER);


    program = new ShaderProgram();

    program->addShaders(*vertexShader, *fragmentShader, *geometryShader);
    program->use();
}


LambertianMaterial::~LambertianMaterial()
{
}


Shader* LambertianMaterial::getShader()
{
    return fragmentShader;
}


void LambertianMaterial::use()
{
    program->use();
}


void LambertianMaterial::updateUniforms(Scene* scene)
{
    program->use();
    glUniform3fv(glGetUniformLocation(program->getId(), "camPos"), 1, (GLfloat*) &scene->getCamera()->pos);
    glUniformMatrix4fv(glGetUniformLocation(program->getId(), "transform"), 1, GL_TRUE, (float*)&scene->getCamera()->getMatrix().m_val);
    GLuint kdLocation = glGetUniformLocation(program->getId(), "Kd");
    glUniform3f(kdLocation, Kd.x, Kd.y, Kd.z);
}


Shader* LambertianMaterial::fragmentShader = nullptr;
Shader* LambertianMaterial::vertexShader = nullptr;
Shader* LambertianMaterial::geometryShader = nullptr;
