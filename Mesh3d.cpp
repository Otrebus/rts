#include "Mesh3d.h"
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include "Matrix4.h"
#include "Math.h"

Mesh3d::Mesh3d()
{
    if(!vertexShader)
        vertexShader = new Shader("vertexShader.vert", GL_VERTEX_SHADER);
    if(!geometryShader)
        geometryShader = new Shader("geometryShader.geom", GL_GEOMETRY_SHADER);
}


Mesh3d::Mesh3d(std::vector<Vertex3d> vertices, std::vector<int> triangles, Material* material)
{
    this->material = material;
    this->v = vertices;
    this->triangles = triangles;
}


Mesh3d::~Mesh3d()
{
}


void Mesh3d::setUp(Scene* s)
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
    auto s = scene->getShaderProgramManager();
    auto program = s->getProgram(this->material->getShader(), getGeometryShader(), getVertexShader());
    scene->setShaderProgram(program);
    program->use();

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, triangles.size(), GL_UNSIGNED_INT, 0);
}

void Mesh3d::updateUniforms()
{
    auto s = scene->getShaderProgramManager();
    auto program = s->getProgram(this->material->getShader(), getGeometryShader(), getVertexShader());
    scene->setShaderProgram(program);

    program->use();

    auto perspM = scene->getCamera()->getMatrix();
    auto transM = getTranslationMatrix(pos);
    auto dirM = getDirectionMatrix(dir, up);

    auto matrix = transM*dirM;

    glUniformMatrix4fv(glGetUniformLocation(program->getId(), "modelViewMatrix"), 1, GL_TRUE, (float*)matrix.m_val);
    glUniformMatrix4fv(glGetUniformLocation(program->getId(), "projectionMatrix"), 1, GL_TRUE, (float*)perspM.m_val);
    glUniformMatrix4fv(glGetUniformLocation(program->getId(), "normalMatrix"), 1, GL_TRUE, (float*)getNormalMatrix(matrix).m_val);

    this->material->updateUniforms(scene);
}

Shader* Mesh3d::getGeometryShader() const
{
    return geometryShader;
}

Shader* Mesh3d::getVertexShader() const
{
    return vertexShader;
}

Shader* Mesh3d::vertexShader = nullptr;
Shader* Mesh3d::geometryShader = nullptr;
