#define NOMINMAX

#include <cmath>
#include <stdio.h>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Shader.h"
#include "ShaderProgram.h"
#include "Utils.h"
#include "Camera.h"
#include "Matrix4.h"
#include "ObjReader.h"
#include "Model.h"
#include "Input.h"
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

    auto model = ReadFromFile("CornellBox-Original.obj");

    int success;
    char infoLog[512];

    glEnable(GL_DEPTH_TEST);

    auto startTime = glfwGetTime();

    int panningX = 0, panningY = 0;
    bool panning = false;
    real theta = 2.2, phi = -2.6;
    real startTheta = 0, startPhi = 0;

    initInput(window);

    Camera cam;
    cam.pos = { -1, 1, 0 };
    CameraControl cameraControl(0, 0, false, 2.2, -2.6, &cam);
    real time = glfwGetTime();

    model.Setup();

    while (!glfwWindowShouldClose(window)) {
        auto prevTime = time;
        time = glfwGetTime();
       
        auto T = getCameraMatrix( cam.pos, cam.pos + cam.dir, 59, 16.0/10.0);

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        model.SetTransformationMatrix(T);
        model.SetCameraPosition(cam.pos);
        model.Draw();
        glfwSwapBuffers(window);

        glfwPollEvents();
        
        handleInput(window, prevTime, time, cameraControl);
    }

    glfwTerminate();
    return 0;
}
