#include "Line.h"
#include "Shader.h"
#include "Math.h"


Line3d::Line3d() : VAO(0), VBO(0) {}

Line3d::Line3d(const std::vector<Vector3>& vertices) : VAO(0), VBO(0) {
        for (const auto& vertex : vertices) {
            vertexData.push_back(vertex.x);
            vertexData.push_back(vertex.y);
            vertexData.push_back(vertex.z);
    }
    if(!fragmentShader)
        fragmentShader = new Shader("line.frag", GL_FRAGMENT_SHADER);
    if(!vertexShader)
        vertexShader = new Shader("line.vert", GL_VERTEX_SHADER);
}

Line3d::~Line3d() {
    tearDown();
}

void Line3d::setUp(Scene* scene) {
    this->scene = scene;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void Line3d::draw() {
    auto s = scene->getShaderProgramManager();
    auto program = s->getProgram(fragmentShader, vertexShader);
    scene->setShaderProgram(program);
    program->use();
    //program->use();

    auto perspM = scene->getCamera()->getMatrix();

    glUniformMatrix4fv(glGetUniformLocation(program->getId(), "transform"), 1, GL_TRUE, (float*)(&perspM.m_val));
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINE_STRIP, 0, vertexData.size()/3);
}

void Line3d::tearDown() {
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}

Shader* Line3d::fragmentShader = nullptr;
Shader* Line3d::vertexShader = nullptr;


// Todo: solve this more elegantly with some base class or something


Line2d::Line2d() : VAO(0), VBO(0), color({0.85, 0, 0}) {}

Line2d::Line2d(const std::vector<Vector2>& vertices) : VAO(0), VBO(0),color({0.85, 0, 0}) {
        for (const auto& vertex : vertices) {
            vertexData.push_back(vertex.x);
            vertexData.push_back(vertex.y);
    }
    if(!fragmentShader)
        fragmentShader = new Shader("line.frag", GL_FRAGMENT_SHADER);
    if(!vertexShader)
        vertexShader = new Shader("line.vert", GL_VERTEX_SHADER);
}

Line2d::~Line2d() {
    tearDown();
}

void Line2d::setUp(Scene* scene) {
    this->scene = scene;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void Line2d::draw() {
    auto s = scene->getShaderProgramManager();
    auto program = s->getProgram(fragmentShader, vertexShader);
    scene->setShaderProgram(program);
    program->use();
    //program->use();

    glUniformMatrix4fv(glGetUniformLocation(program->getId(), "transform"), 1, GL_TRUE, (float*)(&identityMatrix.m_val));
    glUniform3fv(glGetUniformLocation(program->getId(), "Kd"), 1, (float*)(&color));
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINE_STRIP, 0, vertexData.size()/2);
}

void Line2d::tearDown() {
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}

Shader* Line2d::fragmentShader = nullptr;
Shader* Line2d::vertexShader = nullptr;
