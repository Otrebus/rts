#include "Console.h"
#include "Scene.h"
#include "Shader.h"
#include "Math.h"
#include "InputManager.h"
#include "ConsoleSettings.h"
#include "Font.h"


ConsoleHistoryEntry::ConsoleHistoryEntry(std::string entry, HistoryType type)
{
    this->entry = entry;
    this->type = type;
}


Console::Console(Scene* scene)
{
    vertexShader = new Shader("vertexShader.vert", GL_VERTEX_SHADER);
    geometryShader = new Shader("geometryShader.geom", GL_GEOMETRY_SHADER);
    fragmentShader = new Shader("console.frag", GL_FRAGMENT_SHADER);
    this->scene = scene;
    lastBackspacePress = 0.f;
    backSpaceDelay = 0.2f;
    animStartPos = 0.f;
    animStart = -1.f;
    open = false;
    completionIndex = 0;
    historyIndex = 1;
    tabbing = false;
    commandHistorySize = 0;
    shifting = false;
    
    font = new Font(*scene, "Roboto-Bold.ttf");
}

Console::~Console()
{
    delete material;
}

real Console::getY()
{
    auto t = (real(glfwGetTime())-animStart)/animDuration;
    if(t >= 1)
        t = 1;
    return animStartPos + (1.f - (t*(t-2.f)+1))*(real(!open) - animStartPos);
}

bool Console::isVisible()
{
    return open || real(glfwGetTime()) - animStart < animDuration;
}

void Console::setOpen(bool open)
{
    animStartPos = getY();
    this->open = open;

    animStart = real(glfwGetTime());
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
    std::vector<Vertex3> v = {
        { -1, getY(), 0, 0, 0, 1, 0, 0 },
        { 1, getY(), 0, 0, 0, 1, 0, 0 },
        { 1, 1, 0, 0, 0, 1, 0, 0 },
        { -1, 1, 0, 0, 0, 1, 0, 0 }
    };

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex3)*v.size(), v.data(), GL_STATIC_DRAW);
    auto s = scene->getShaderProgramManager();
    auto program = s->getProgram(fragmentShader, geometryShader, vertexShader);
    scene->setShaderProgram(program);
    program->use();

    auto matrix = identityMatrix;

    glUniformMatrix4fv(glGetUniformLocation(program->getId(), "modelViewMatrix"), 1, GL_TRUE, (float*)matrix.m_val);
    glUniformMatrix4fv(glGetUniformLocation(program->getId(), "projectionMatrix"), 1, GL_TRUE, (float*)matrix.m_val);
    glUniformMatrix4fv(glGetUniformLocation(program->getId(), "normalMatrix"), 1, GL_TRUE, (float*)matrix.m_val);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    int i = 1;
    font->draw(*scene, textInput + "|", Vector2(-0.98f, getY() + ++i*textSize*1.05f), textSize, Vector3(1.f, 1.f, 0.f));
    for(auto it = history.rbegin(); it < history.rend(); it++)
    {
        Vector3 color = it->type == ConsoleHistoryEntry::Input ? Vector3(0.8f, 0.8f, 0.8f) : Vector3(0.6f, 0.6f, 0.6f);
        font->draw(*scene, it->entry, Vector2(-0.98f, getY() + ++i*textSize*1.05f), textSize, color);
    }
}

void Console::handleInput(const Input& input)
{
    if(input.key == GLFW_KEY_BACKSPACE)
    {
        if(input.stateStart == InputType::KeyPress && textInput.size())
        {
            lastBackspacePress = real(glfwGetTime());
            textInput.pop_back();
        }
        if(input.stateStart == InputType::KeyHold)
        {
            if(real(glfwGetTime()) - lastBackspacePress > backSpaceDelay) // TODO: find the actual repetition rate
            {
                lastBackspacePress = real(glfwGetTime());
                backSpaceDelay = 0.05f;
                if(textInput.size())
                    textInput.pop_back();
            }
        }
        if(input.stateEnd == InputType::KeyRelease)
        {
            backSpaceDelay = 0.25;
        }
    }
    else if(input.key == GLFW_KEY_ENTER && input.stateStart == InputType::KeyPress)
    {
        if(!textInput.empty())
        {
            history.push_back( { textInput, ConsoleHistoryEntry::Input });
            std::stringstream ss(textInput);
            std::string varName, arg;

            ss >> varName;
            if(!ConsoleSettings::hasVariable(varName))
            {
                history.push_back( { "Unknown variable \"" + varName + "\"", ConsoleHistoryEntry::Output } );
            }
            else if(ss >> arg)
            {
                auto ss2 = std::stringstream(arg);
                real num;
                if(ss2 >> num)
                {
                    if(ss >> arg)
                        history.push_back( { "Syntax error", ConsoleHistoryEntry::Output });
                    else
                    {
                        auto var = ConsoleSettings::getVariable(varName);
                        if(var->type == var->Int)
                            var->var = int(num);
                        else
                            var->var = num;
                        history.push_back( { varName + " set to " + std::to_string(num), ConsoleHistoryEntry::Output } );
                    }
                }
                else
                {
                    history.push_back( { "Syntax error", ConsoleHistoryEntry::Output });
                }
            }
            else
            {
                auto var = ConsoleSettings::getVariable(varName);
                std::visit(
                    [&](auto&& value) {
                        history.push_back({ std::to_string(value), ConsoleHistoryEntry::Output });
                    },
                    var->var
                );
            }
            if(commandHistory.empty() || commandHistory.back() != textInput)
                commandHistory.push_back(textInput);
            textInput = "";
        }
    }
    else if(input.stateStart == InputType::Char && input.key != GLFW_KEY_GRAVE_ACCENT)
    {
        textInput += (char) input.key;
    }
    else if(input.key == GLFW_KEY_TAB && input.stateStart == InputType::KeyPress)
    {
        if(!tabbing)
        {
            completionStrings = ConsoleSettings::getCompletionStrings(textInput);
            tabbing = true;
            completionIndex = 0;
        }
        else
            completionIndex = (completionIndex + (!shifting ? 1 : (int(completionStrings.size()) - 1))) % completionStrings.size();

        if(completionStrings.size())
            textInput = completionStrings[completionIndex] + " ";
        else
            tabbing = false;
    }
    else if(input.key == GLFW_KEY_UP && input.stateStart == InputType::KeyPress)
    {
        if(!commandHistory.empty())
        {
            if(historyIndex >= int(commandHistory.size()))
            {
                historyIndex = int(commandHistory.size());
            }
            historyIndex = std::max(historyIndex-1, 0);
            textInput = commandHistory[historyIndex];
        }
    }
    else if(input.key == GLFW_KEY_DOWN && input.stateStart == InputType::KeyPress)
    {
        if(!commandHistory.empty())
        {
            if(historyIndex < commandHistory.size()-1)
            {
                historyIndex++;
                textInput = commandHistory[historyIndex];
            }
        }
    }
    if(input.key != GLFW_KEY_TAB && input.key != GLFW_KEY_LEFT_SHIFT && input.key != GLFW_KEY_RIGHT_SHIFT && input.stateStart == InputType::KeyPress)
    {
        tabbing = false;
    }
    if(input.key != GLFW_KEY_UP && input.key != GLFW_KEY_DOWN && input.stateStart == InputType::KeyPress)
    {
        historyIndex = int(history.size());
    }
    if(input.key == GLFW_KEY_LEFT_SHIFT || input.key == GLFW_KEY_RIGHT_SHIFT)
    {
        shifting = input.stateEnd != InputType::KeyRelease;
    }
}
