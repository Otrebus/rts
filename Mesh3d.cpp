#include "Mesh3d.h"
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include "Matrix4.h"

Mesh3d::Mesh3d()
{
}


Mesh3d::Mesh3d(std::vector<Vertex3d> vertices, std::vector<int> triangles, Material* material)
{
    this->material = material;
    this->v = vertices;
    this->triangles = triangles;
}


void Mesh3d::setup(Scene* s)
{
    scene = s;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex3d)*v.size(), v.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*triangles.size(), triangles.data(), GL_STATIC_DRAW);
}

void Mesh3d::tearDown(Scene* s)
{
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
}

void Mesh3d::draw()
{
    material->use();
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, triangles.size(), GL_UNSIGNED_INT, 0);
}

void Mesh3d::updateUniforms()
{
    this->material->updateUniforms(scene);
}