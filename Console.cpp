#include "Console.h"
#include "Scene.h"
#include "Shader.h"
#include "Math.h"
#include "Text.h"
#include "InputManager.h"

Console::Console(Scene* scene)
{
    vertexShader = new Shader("vertexShader.vert", GL_VERTEX_SHADER);
    geometryShader = new Shader("geometryShader.geom", GL_GEOMETRY_SHADER);
    fragmentShader = new Shader("lambertian.frag", GL_FRAGMENT_SHADER);
    this->scene = scene;
}

Console::~Console()
{
    delete material;
}

void Console::init()
{
    std::vector<Vertex3> v = {
        { -1, -0, 0, 0, 0, 1, 0, 0 },
        { 1, -0, 0, 0, 0, 1, 0, 0 },
        { 1, 1, 0, 0, 0, 1, 0, 0 },
        { -1, 1, 0, 0, 0, 1, 0, 0 }
    };
    std::vector<int> triangles = { 0, 1, 2, 0, 2, 3 };

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

void Console::draw()
{
    auto s = scene->getShaderProgramManager();
    auto program = s->getProgram(fragmentShader, geometryShader, vertexShader);
    scene->setShaderProgram(program);
    program->use();

    auto matrix = identityMatrix;

    glUniformMatrix4fv(glGetUniformLocation(program->getId(), "modelViewMatrix"), 1, GL_TRUE, (float*)matrix.m_val);
    glUniformMatrix4fv(glGetUniformLocation(program->getId(), "projectionMatrix"), 1, GL_TRUE, (float*)matrix.m_val);
    glUniformMatrix4fv(glGetUniformLocation(program->getId(), "normalMatrix"), 1, GL_TRUE, (float*)matrix.m_val);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    std::vector<std::string> rows;
    rows.push_back("I am text");
    rows.push_back("gj394539efwhpq38rfff34g34hpnwe;fhu");
    rows.push_back("I am going to be some sort of text that is something in nature||");

    int i = 1;
    drawText(textInput, Vector2(-0.98, bottomPos + ++i*textSize*1.05), textSize, Vector3(1, 1, 0));
    for(auto it = rows.rbegin(); it < rows.rend(); it++)
    {
        auto row = *it;
        drawText(row, Vector2(-0.98, bottomPos + ++i*textSize*1.05), textSize, Vector3(0, 1, 0));
    }
}

void Console::handleInput(const Input& input)
{
    if(input.stateStart == InputType::KeyPress && input.key == GLFW_KEY_BACKSPACE)
    {
        if(textInput.size())
            textInput.pop_back();
    }
    else if(input.stateStart == InputType::Char)
    {
        textInput += (char) input.key;
    }
}