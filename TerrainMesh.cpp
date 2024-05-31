#include "TerrainMesh.h"
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include "Matrix4.h"
#include "TerrainMaterial.h"

TerrainMesh::TerrainMesh(std::vector<MeshVertex3d> vertices, std::vector<int> triangles, Material* material)
{
    this->material = material;
    this->v = vertices;
    this->triangles = triangles;
    if(!geometryShader)
        geometryShader = new Shader("geometryShader.geom", GL_GEOMETRY_SHADER);
    if(!terrainVertexShader)
        terrainVertexShader = new Shader("terrain.vert", GL_VERTEX_SHADER);
}

TerrainMesh::~TerrainMesh()
{
}

void TerrainMesh::init(Scene* s)
{
    scene = s;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(MeshVertex3d)*v.size(), v.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 1, GL_INT, GL_FALSE, 9 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*triangles.size(), triangles.data(), GL_STATIC_DRAW);
}

void TerrainMesh::updateUniforms()
{
    auto s = scene->getShaderProgramManager();
    auto program = s->getProgram(this->material->getShader(), getGeometryShader(), getVertexShader());
    scene->setShaderProgram(program);

    program->use();

    auto perspM = scene->getCamera()->getMatrix();
    auto matrix = getTransformationMatrix();

    glUniformMatrix4fv(glGetUniformLocation(program->getId(), "projectionMatrix"), 1, GL_TRUE, (float*)perspM.m_val);

    this->material->updateUniforms(scene);
}

Shader* TerrainMesh::getVertexShader() const
{
    return terrainVertexShader;
}

void TerrainMesh::setFlat(bool flatness)
{
    ((TerrainMaterial*) material)->setFlat(flatness);
}

Shader* TerrainMesh::terrainVertexShader = nullptr;
