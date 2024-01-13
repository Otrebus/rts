#include "Line3d.h"
#include "Shader.h"


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

    program = new ShaderProgram();

    program->addShaders(*vertexShader, *fragmentShader);
}

Line3d::~Line3d() {
    tearDown();
}

void Line3d::setup(Scene* scene) {
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
    program->use();
    glUniformMatrix4fv(glGetUniformLocation(program->getId(), "transform"), 1, GL_TRUE, (float*)&scene->getCamera()->getMatrix(59, 16./10.).m_val);
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, vertexData.size()/3);
}

void Line3d::tearDown() {
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}

Shader* Line3d::fragmentShader = nullptr;
Shader* Line3d::vertexShader = nullptr;

