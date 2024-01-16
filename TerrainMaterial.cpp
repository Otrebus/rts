#include "TerrainMaterial.h"
#include "Vector3.h"
#include "Shader.h"
#include "Scene.h"


TerrainMaterial::TerrainMaterial()
{     
    if(!fragmentShader) // Or rather !initialized or something
        fragmentShader = new Shader("terrain.frag", GL_FRAGMENT_SHADER);
    if(!vertexShader)
        vertexShader = new Shader("terrain.vert", GL_VERTEX_SHADER);
    if(!geometryShader)
        geometryShader = new Shader("geometryShader.geom", GL_GEOMETRY_SHADER);

    program = new ShaderProgram();

    program->addShaders(*vertexShader, *fragmentShader, *geometryShader);
    program->use();
}


TerrainMaterial::~TerrainMaterial()
{
}


Shader* TerrainMaterial::getShader()
{
    return fragmentShader;
}


void TerrainMaterial::use()
{
    program->use();
}


void TerrainMaterial::updateUniforms(Scene* scene)
{
    program->use();
    glUniform3fv(glGetUniformLocation(program->getId(), "camPos"), 1, (GLfloat*) &scene->getCamera()->pos);
    glUniformMatrix4fv(glGetUniformLocation(program->getId(), "transform"), 1, GL_TRUE, (float*)&scene->getCamera()->getMatrix().m_val);
}


Shader* TerrainMaterial::fragmentShader = nullptr;
Shader* TerrainMaterial::vertexShader = nullptr;
Shader* TerrainMaterial::geometryShader = nullptr;
