#include "Math.h"
#include "Mesh3d.h"

Mesh3d::Mesh3d()
{
    scene = nullptr;
    if(!vertexShader)
        vertexShader = new Shader("vertexShader.vert", GL_VERTEX_SHADER);
    if(!geometryShader)
        geometryShader = new Shader("geometryShader.geom", GL_GEOMETRY_SHADER);
}

Mesh3d::Mesh3d(std::vector<Vertex3> vertices, std::vector<int> triangles, Material* material)
{
    this->material = material;
    this->v = vertices;
    this->triangles = triangles;
    this->nTriangles = int(triangles.size());
    if(!vertexShader)
        vertexShader = new Shader("vertexShader.vert", GL_VERTEX_SHADER);
    if(!geometryShader)
        geometryShader = new Shader("geometryShader.geom", GL_GEOMETRY_SHADER);
}

Mesh3d::Mesh3d(Mesh3d& mesh)
{
    this->material = mesh.material->clone();
    this->v = mesh.v;
    this->triangles = mesh.triangles;
    this->nTriangles = mesh.nTriangles;
    this->VAO = mesh.VAO;
    this->VBO = mesh.VBO;
    this->EBO = mesh.EBO;
}

Mesh3d::~Mesh3d()
{
}

void Mesh3d::init()
{
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex3)*v.size(), v.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*triangles.size(), triangles.data(), GL_STATIC_DRAW);
}

void Mesh3d::tearDown()
{
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}

void Mesh3d::draw(Material* mat)
{
    auto material = mat ? mat : this->material;
    auto s = scene->getShaderProgramManager();
    auto program = s->getProgram(material->getShader(), getGeometryShader(), getVertexShader());
    scene->setShaderProgram(program);
    program->use();

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, nTriangles, GL_UNSIGNED_INT, 0);
}

void Mesh3d::updateUniforms(Material* mat)
{
    auto material = mat ? mat : this->material;
    auto s = scene->getShaderProgramManager();
    auto program = s->getProgram(material->getShader(), getGeometryShader(), getVertexShader());
    scene->setShaderProgram(program);

    program->use();

    auto perspM = scene->getCamera()->getMatrix();
    auto matrix = getTransformationMatrix();

    glUniformMatrix4fv(glGetUniformLocation(program->getId(), "modelViewMatrix"), 1, GL_TRUE, (float*)matrix.m_val);
    glUniformMatrix4fv(glGetUniformLocation(program->getId(), "projectionMatrix"), 1, GL_TRUE, (float*)perspM.m_val);
    glUniformMatrix4fv(glGetUniformLocation(program->getId(), "normalMatrix"), 1, GL_TRUE, (float*)getNormalMatrix(matrix).m_val);

    material->updateUniforms(scene);
}

Shader* Mesh3d::getGeometryShader() const
{
    return geometryShader;
}

Shader* Mesh3d::getVertexShader() const
{
    return vertexShader;
}

void Mesh3d::transform(Matrix4 matrix)
{
    transformationMatrix = matrix;
    auto normalMatrix = getNormalMatrix(matrix);
    for(auto& p : v)
    {
        p.pos *= matrix;
        p.normal *= normalMatrix;
    }
}

Shader* Mesh3d::vertexShader = nullptr;
Shader* Mesh3d::geometryShader = nullptr;
