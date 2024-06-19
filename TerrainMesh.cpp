#include "Matrix4.h"
#include "TerrainMaterial.h"
#include "TerrainMesh.h"
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

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

void TerrainMesh::init()
{
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(MeshVertex3d) * v.size(), v.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex3d), (void*)offsetof(MeshVertex3d, pos));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex3d), (void*)offsetof(MeshVertex3d, normal));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(MeshVertex3d), (void*)offsetof(MeshVertex3d, tex));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 1, GL_INT, GL_FALSE, sizeof(MeshVertex3d), (void*)offsetof(MeshVertex3d, selected));
    glEnableVertexAttribArray(3);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * triangles.size(), triangles.data(), GL_STATIC_DRAW);
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
    ((TerrainMaterial*)material)->setFlat(flatness);
}

Shader* TerrainMesh::terrainVertexShader = nullptr;
