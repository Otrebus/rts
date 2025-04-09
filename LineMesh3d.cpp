#include "LineMesh3d.h"
#include "LineModelMaterial.h"

LineMesh3d::LineMesh3d()
{
    if(!vertexShader)
        vertexShader = new Shader("lineModel.vert", GL_VERTEX_SHADER);
    if(!geometryShader)
        geometryShader = new Shader("lineModel.geom", GL_GEOMETRY_SHADER);
}


LineMesh3d::~LineMesh3d()
{
}


LineMesh3d::LineMesh3d(std::vector<Vector3> vertices, std::vector<std::pair<int, int>> lines, Material* material, int lineWidth) : lineWidth(lineWidth)
{
    this->material = material ? material : new LineModelMaterial(Vector3(0, 1, 0));
    this->v = vertices;
    this->lines = lines;
    if(!vertexShader)
        vertexShader = new Shader("lineModel.vert", GL_VERTEX_SHADER);
    if(!geometryShader)
        geometryShader = new Shader("lineModel.geom", GL_GEOMETRY_SHADER);
}


void LineMesh3d::init()
{
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3)*v.size(), v.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &EBO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    for(const auto& line : lines)
    {
        indices.push_back(static_cast<GLuint>(line.first));
        indices.push_back(static_cast<GLuint>(line.second));
    }

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void LineMesh3d::tearDown()
{
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
}

void LineMesh3d::draw(Material *mat)
{
    auto material = mat ? mat : this->material;
    auto s = scene->getShaderProgramManager();
    auto program = s->getProgram(material->getShader(), getGeometryShader(), getVertexShader());
    scene->setShaderProgram(program);
    program->use();

    glLineWidth(2);
    glBindVertexArray(VAO);
    //glDrawArrays(GL_LINE_STRIP, 0, static_cast<GLsizei>(indices.size())/2);
    //glDrawArrays(GL_LINES, 0, 4);
    glDrawElements(GL_LINES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
}

void LineMesh3d::updateUniforms(Material* mat)
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
