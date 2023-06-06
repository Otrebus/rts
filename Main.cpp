#define NOMINMAX

#include <cmath>
#include <stdio.h>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Shader.h"
#include "Utils.h"
#include "Matrix4.h"
#include "ObjReader.h"
#include "Model.h"
#include <queue>


void checkError() {
GLenum error;
    error = glGetError();

    if (error != GL_NO_ERROR)
    {
        std::cout << "not working" << std::endl;
    }

    if (error == GL_INVALID_OPERATION)
    {
        std::cout << "INVALID OPERATION" << std::endl;
    }
    if (error == GL_NO_ERROR)
    {
        std::cout << "No Error! -> CODE CHECKING <-" << std::endl;
    }

    if (error == GL_INVALID_VALUE)
    {
        std::cout << "NO VALUE" << std::endl;
    }
}

static const real pi = std::acos(-1);

struct Input
{
    double time;
    enum Type
    {
        MousePress, KeyPress, MousePosition
    } type;
    int key, state;
    real posX, posY;
};

struct InputQueue // Tightly coupled with glfw
{
    std::queue<Input> queue;
    bool mouseState[8];
    bool keyState[255];
    real posX, posY;

    double timeMouse[8];
    double timeKey[255];

    InputQueue()
    {
        std::memset(mouseState, 0, sizeof(mouseState));
        std::memset(keyState, 0, sizeof(keyState));
    }

    void addKeyInput(double time, int key, int state)
    {
        queue.push({ time, Input::Type::KeyPress, key, state });
    }

    void addMouseInput(double time, int key, int state)
    {
        queue.push({ time, Input::Type::MousePress, key, state });
    }

    void addMousePosition(double time, real x, real y)
    {
        queue.push({ time, Input::Type::MousePosition, 0, 0, x, y });
    }

    bool hasInput()
    {
        return !queue.empty();
    }

    Input pop()
    {
        auto input = queue.front();
        queue.pop();
        if(input.type == Input::Type::MousePress) {
            timeMouse[input.key] = input.time;
            mouseState[input.key] = input.state;
        }

        if(input.type == Input::Type::KeyPress) {
            timeKey[input.key] = input.time;
            keyState[input.key] = input.state;
        }
        
        if(input.type == Input::Type::MousePosition) {
            std::cout << input.posX << " " << input.posY << std::endl;
            posX = input.posX, posY = input.posY;
        }
        return input;
    }
} inputQueue;


int main()
{
    if (!glfwInit()) {
        printf("failed to initialize GLFW.\n");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);

    auto window = glfwCreateWindow(1000, 600, "awesome", nullptr, nullptr);
    if (!window) {
        return -1;
    }

    glfwMakeContextCurrent(window);
    if (gl3wInit()) {
        printf("failed to initialize OpenGL\n");
        return -1;
    }

    float vertices[] = {
      -0.5f, -0.5f, -1.0f, 1, 1, 0, 0.0, 0.0,
       0.5f, -0.5f, -1.0f, 0, 1, 1, 1.0, 0.0,
       0.5f,  0.5f, -1.0f, 1, 0, 1, 1.0, 1.0,
       -0.5f, 0.5f, -1.0f, 1, 0, 0, 0.0, 1.0
    };

    float vertices2[] = {
      -0.5f, -0.5f, -1.5f, 1, 1, 0, 0.0, 0.0,
       0.5f, -0.5f, -1.5f, 0, 1, 1, 1.0, 0.0,
       0.5f,  0.5f, -1.5f, 1, 0, 1, 1.0, 1.0,
       -0.5f, 0.5f, -1.5f, 1, 0, 0, 0.0, 1.0
    };

    unsigned int VBO, VAO, VBO2, VAO2, EBO, EBO2;

    auto model = ReadFromFile("CornellBox-Original.obj");

    /*unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);*/

    Shader vertexShader("vertex.glsl", GL_VERTEX_SHADER);
    Shader fragmentShader("fragment.glsl", GL_FRAGMENT_SHADER);

    /*unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);*/

    /*unsigned int shaderProgram;
    shaderProgram = glCreateProgram();*/

    ShaderProgram s;
    s.AddShaders(vertexShader, fragmentShader);
    s.Use();

    int success;
    char infoLog[512];
    /*glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }*/

    //glAttachShader(shaderProgram, vertexShader);
    //glAttachShader(shaderProgram, fragmentShader);
    //glLinkProgram(shaderProgram);
    //glUseProgram(shaderProgram);

    //auto timeUniformLocation = glGetUniformLocation(shaderProgram, "u_time");

    auto [data, width, height] = readBMP("wall.bmp");
    auto [data2, width2, height2] = readBMP("grass.bmp");
    unsigned int textures[2];
    glGenTextures(2, textures); 

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data.data());

    glBindTexture(GL_TEXTURE_2D, textures[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width2, height2, 0, GL_RGB, GL_UNSIGNED_BYTE, data2.data());


    /*unsigned int indices[2][3] = { {
        0, 1, 2,
        }, { 0, 2, 3 } };*/

    unsigned int indices[6] = { 0, 1, 2, 0, 2, 3 };
    unsigned int indices2[6] = { 0, 1, 2, 0, 2, 3 };

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindVertexArray(VAO);

    checkError();
    glUniform1i(glGetUniformLocation(s.GetId(), "text"), 0);

    glGenVertexArrays(1, &VAO2);
    glBindVertexArray(VAO2);

    glGenBuffers(1, &VBO2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glGenBuffers(1, &EBO2);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices2), indices2, GL_STATIC_DRAW);

    //glGenVertexArrays(1, &VAO2);
    //glBindVertexArray(VAO2);

    checkError();

    //glGenBuffers(1, &VBO2);
    //glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //glGenBuffers(1, &EBO2);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[1]), indices[1], GL_STATIC_DRAW);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textures[1]);

    glEnable(GL_DEPTH_TEST);

    //glDepthFunc(GL_LESS);
    
    auto key_callback = [] (GLFWwindow* window, int key, int scancode, int action, int mods) {
        /*if (key == GLFW_KEY_F && action == GLFW_PRESS)
        {
            blah++;
            std::cout << blah << std::endl;
        }*/
        inputQueue.addKeyInput(glfwGetTime(), key, action);
    };

    auto mouseButton_callback = [] (GLFWwindow* window, int button, int action, int mods) {
        inputQueue.addMouseInput(glfwGetTime(), button, action);
    };

    auto cursor_position_callback = [] (GLFWwindow* window, double xpos, double ypos)
    {         
        inputQueue.addMousePosition(glfwGetTime(), xpos, ypos);
    };

    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouseButton_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);

    auto startTime = glfwGetTime();

    int panningX = 0, panningY = 0;
    bool panning = false;
    real theta = 0, phi = 0;
    real startTheta = 0, startPhi = 0;


    while (!glfwWindowShouldClose(window)) {
        auto time = glfwGetTime();
        //glUniform1f(timeUniformLocation, time);
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        //trans2.m_val[1][3] = (time-startTime)*0.05;
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(VAO);

        Vector3 f(0, 0, 1), r(1, 0, 0), u(0, 1, 0);
        Vector3 h = f*std::sin(theta) + r*std::cos(theta);
        Vector3 dir = u*std::sin(phi) + h*std::cos(phi);

        /*std::cout << phi << " " << theta << std::endl;
        std::cout << dir;*/


        auto T = getCameraMatrix( { -1, 1, 0 }, Vector3{ -1, 1, 0 } + dir, 59, 16.0/10.0);
        // auto T = getCameraMatrix( { -1, 1, 0 }, Vector3{ -1, 1, 0 } + dir, 59, 16.0/10.0);
        glUniformMatrix4fv(glGetUniformLocation(s.GetId(), "transform"), 1, GL_TRUE, (float*) T.m_val);
        glBindTexture(GL_TEXTURE_2D, textures[0]);

        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        //glBindVertexArray(VAO2);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glBindVertexArray(VAO2);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[1]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        //glDrawArrays(GL_TRIANGLES, 0, 6);

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }

        int test = 0;

        //for(int i = 0; i < 1000000000; i++) {
        //    test++;
        //}

        glfwSwapBuffers(window);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        while(inputQueue.hasInput())
        {
            auto input = inputQueue.pop();
            if(input.type == Input::Type::KeyPress)
            {
                if(input.state == GLFW_PRESS || input.state == GLFW_RELEASE)
                    std::cout << input.time << ": " << (std::string("key was ") + ((input.state == GLFW_PRESS) ? "pressed" : "released")) << std::endl;
            }
        }

        if(!panning && inputQueue.mouseState[GLFW_MOUSE_BUTTON_1]) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);

            std::cout << panningY << " " << inputQueue.posY << std::endl;
            startTheta = theta;
            startPhi = phi;
            panningX = xpos;
            panningY = ypos;
            panning = true;
        }

        if(panning && inputQueue.mouseState[GLFW_MOUSE_BUTTON_1] == GLFW_RELEASE) {
            panningX = inputQueue.posX;
            panningY = inputQueue.posY;
            panning = false;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }

        if(panning) {
            phi = startPhi + (inputQueue.posY - panningY)/300;
            theta = startTheta + (inputQueue.posX - panningX)/300;
        }

        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
