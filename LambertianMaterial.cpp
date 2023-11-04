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

    program = new ShaderProgram();

    program->AddShaders(*vertexShader, *fragmentShader);
    program->Use();
}


Shader* LambertianMaterial::GetShader()
{
    return fragmentShader;
}


void LambertianMaterial::Use()
{
    program->Use();
}


void LambertianMaterial::UpdateUniforms(Scene* scene)
{
    program->Use();
    glUniform3fv(glGetUniformLocation(program->GetId(), "camPos"), 1, (GLfloat*) &scene->GetCamera()->pos);
    glUniformMatrix4fv(glGetUniformLocation(program->GetId(), "transform"), 1, GL_TRUE, (float*)&scene->GetCamera()->GetMatrix(59, 16./10.).m_val);
    GLuint kdLocation = glGetUniformLocation(program->GetId(), "Kd");
    glUniform3f(kdLocation, Kd.x, Kd.y, Kd.z);
}


Shader* LambertianMaterial::fragmentShader = nullptr;
Shader* LambertianMaterial::vertexShader = nullptr;
