#include "LineMesh3d.h"

LineMesh3d::LineMesh3d()
{
    if(!vertexShader)
        vertexShader = new Shader("lineModel.vert", GL_VERTEX_SHADER);
}


LineMesh3d::~LineMesh3d()
{
}


LineMesh3d::LineMesh3d(std::vector<Vector3> vertices, std::vector<std::pair<int, int>> lines, Material* material, int lineWidth) : lineWidth(lineWidth)
{
    this->material = material;
    this->v = vertices;
    this->lines = lines;
    if(!vertexShader)
        vertexShader = new Shader("lineModel.vert", GL_VERTEX_SHADER);
}


void LineMesh3d::init(Scene* s)
{
    scene = s;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3)*v.size(), v.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    GLuint EBO;
    glGenBuffers(1, &EBO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    for(const auto& line : lines)
    {
        indices.push_back(static_cast<GLuint>(line.first));
        indices.push_back(static_cast<GLuint>(line.second));
    }

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);
}

void LineMesh3d::tearDown(Scene* s)
{
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
}

void LineMesh3d::draw(Material *mat)
{
    auto s = scene->getShaderProgramManager();
    auto program = s->getProgram(this->material->getShader(), getVertexShader());
    scene->setShaderProgram(program);
    program->use();

    glLineWidth(2);
    glBindVertexArray(VAO);
    glDrawElements(GL_LINES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
}

void LineMesh3d::updateUniforms(Material* mat)
{
    auto material = mat ? mat : this->material;
    auto s = scene->getShaderProgramManager();
    auto program = s->getProgram(material->getShader(), getVertexShader());
    scene->setShaderProgram(program);

    program->use();

    auto perspM = scene->getCamera()->getMatrix();
    auto matrix = getTransformationMatrix();

    glUniformMatrix4fv(glGetUniformLocation(program->getId(), "modelViewMatrix"), 1, GL_TRUE, (float*)matrix.m_val);
    glUniformMatrix4fv(glGetUniformLocation(program->getId(), "projectionMatrix"), 1, GL_TRUE, (float*)perspM.m_val);

    material->updateUniforms(scene);
}

Shader* LineMesh3d::getGeometryShader() const
{
    return geometryShader;
}

Shader* LineMesh3d::getVertexShader() const
{
    return vertexShader;
}


Shader* LineMesh3d::vertexShader = nullptr;
Shader* LineMesh3d::geometryShader = nullptr;
