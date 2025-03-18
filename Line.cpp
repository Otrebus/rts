#include "Line.h"
#include "Math.h"


Line::Line() : VAO(0), VBO(0)
{
    if(!fragmentShader)
        fragmentShader = new Shader("line.frag", GL_FRAGMENT_SHADER);
    if(!vertexShader)
        vertexShader = new Shader("line.vert", GL_VERTEX_SHADER);
    if(!geometryShader)
        geometryShader = new Shader("lineGeoShader.geom", GL_GEOMETRY_SHADER);
}

Line::Line(const std::vector<Vector3>& vertices)
{
    for(const auto& vertex : vertices)
    {
        vertexData.push_back(vertex.x);
        vertexData.push_back(vertex.y);
        vertexData.push_back(vertex.z);
        vertexData.push_back(-1.0f);
    }
}

Line3d::Line3d()
{
}

Line3d::~Line3d()
{
    tearDown();
}

void Line3d::init(Scene* scene)
{
    this->scene = scene;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
}

void Line3d::setDashed(bool b)
{
    dashed = b;
}

void Line3d::setVertices(const std::vector<Vector3>& vertices)
{
    vertexData.clear();
    real dist = 0;
    for(int i = 0; i < vertices.size(); i++)
    {
        const auto& vertex = vertices[i];
        if(i > 0)
            dist += (vertex - vertices[i-1]).length();
        vertexData.push_back(vertex.x);
        vertexData.push_back(vertex.y);
        vertexData.push_back(vertex.z);
        vertexData.push_back(dashed ? dist : -1.0f);
    }
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);
}

void Line3d::setInFront(bool inFront)
{
    this->inFront = inFront;
}

void Line3d::draw()
{
    auto s = scene->getShaderProgramManager();
    auto program = s->getProgram(fragmentShader, geometryShader, vertexShader);
    scene->setShaderProgram(program);
    program->use();

    auto perspM = scene->getCamera()->getMatrix();

    bool depthDestIsEnabled = glIsEnabled(GL_DEPTH_TEST);

    if(inFront)
        glDisable(GL_DEPTH_TEST);
    glUniformMatrix4fv(glGetUniformLocation(program->getId(), "transform"), 1, GL_TRUE, (float*)(&perspM.m_val));
    GLuint kdLocation = glGetUniformLocation(program->getId(), "Kd");
    glUniform4f(kdLocation, color.x, color.y, color.z, color.w);
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINE_STRIP, 0, int(vertexData.size())/4);
    if(inFront && depthDestIsEnabled)
        glEnable(GL_DEPTH_TEST);
}

void Line3d::tearDown()
{
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}

void Line::setColor(Vector4 color)
{
    this->color = color;
}

Line2d::Line2d(const std::vector<Vector2>& vertices)
{
    for(const auto& vertex : vertices)
    {
        vertexData.push_back(vertex.x);
        vertexData.push_back(vertex.y);
        vertexData.push_back(0);
        vertexData.push_back(0);
    }
    if(!fragmentShader)
        fragmentShader = new Shader("line.frag", GL_FRAGMENT_SHADER);
    if(!vertexShader)
        vertexShader = new Shader("line.vert", GL_VERTEX_SHADER);
    if(!geometryShader)
        geometryShader = new Shader("lineGeoShader.geom", GL_GEOMETRY_SHADER);
}

Line3d::Line3d(const std::vector<Vector3>& vertices) : Line(vertices)
{
    if(!fragmentShader)
        fragmentShader = new Shader("line.frag", GL_FRAGMENT_SHADER);   
    if(!vertexShader)
        vertexShader = new Shader("line.vert", GL_VERTEX_SHADER);
    if(!geometryShader)
        geometryShader = new Shader("lineGeoShader.geom", GL_GEOMETRY_SHADER);
}

Line2d::Line2d()
{
}

Line2d::~Line2d()
{
    tearDown();
}

void Line2d::setVertices(const std::vector<Vector2>& vertices)
{
    vertexData.clear();
    for(const auto& vertex : vertices)
    {
        vertexData.push_back(vertex.x);
        vertexData.push_back(vertex.y);
        vertexData.push_back(0);
        vertexData.push_back(0);
    }
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);
}

void Line2d::init(Scene* scene)
{
    this->scene = scene;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, int(vertexData.size())*sizeof(float), vertexData.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
}

void Line2d::draw()
{
    glDisable(GL_DEPTH_TEST);
    auto s = scene->getShaderProgramManager();
    auto program = s->getProgram(fragmentShader, geometryShader, vertexShader);
    scene->setShaderProgram(program);
    program->use();
    
    glUniformMatrix4fv(glGetUniformLocation(program->getId(), "transform"), 1, GL_TRUE, (float*)(&identityMatrix.m_val));
    glUniform4fv(glGetUniformLocation(program->getId(), "Kd"), 1, (float*)(&color));
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINE_STRIP, 0, int(vertexData.size())/4);
    glEnable(GL_DEPTH_TEST);
}

void Line2d::tearDown()
{
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}

Shader* Line::fragmentShader = nullptr;
Shader* Line::vertexShader = nullptr;
Shader* Line::geometryShader = nullptr;
