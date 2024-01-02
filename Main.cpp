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
#include "TextureMaterial.h"
#include "Input.h"
#include "Scene.h"
#include <queue>
#include "Terrain.h"
#include <thread>


void checkError() {
    GLenum error;
    error = glGetError();

    if (error != GL_NO_ERROR)
        std::cout << "Not working" << std::endl;
    if (error == GL_INVALID_OPERATION)
        std::cout << "Invalid operation" << std::endl;
    if (error == GL_NO_ERROR)
        std::cout << "No error" << std::endl;
    if (error == GL_INVALID_VALUE)
        std::cout << "No value" << std::endl;
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

    auto window = glfwCreateWindow(1000, 600, "PolRts", nullptr, nullptr);
    if (!window) {
        return -1;
    }

    glfwMakeContextCurrent(window);
    if (gl3wInit()) {
        printf("failed to initialize OpenGL\n");
        return -1;
    }

    auto model = Model3d("CornellBox-Original.obj");

    TextureMaterial texture("wall.bmp");
    TextureMaterial texture2("grass.bmp");

    int success;
    char infoLog[512];

    glEnable(GL_DEPTH_TEST);

    auto startTime = glfwGetTime();

    initInput(window);

    Camera cam;
    cam.pos = { 0, 0, 3.5 };
    CameraControl cameraControl(0, 0, false, 0, 0, &cam);
    real time = glfwGetTime();

    std::vector<Vertex3d> meshVertices = {
        { -0.5f, 0.5f, 0.5f, 0, 0, 1, 0, 0 },
        { 0.5f, 0.5f, 0.5f, 0, 0, 1, 1, 0, },
        { 0.5f,  1.5f, 0.5f, 0, 0, 1, 1, 1, },
        { -0.5f, 1.5f, 0.5f, 0, 0, 1, 0, 1, }
    };

    std::vector<Vertex3d> meshVertices2 = {
        { 0.5f, 0.5f, 0.5f, 0, 0, 1, 0, 0 },
        { 1.5f, 0.5f, 0.5f, 0, 0, 1, 1, 0, },
        { 1.5f,  1.5f, 0.5f, 0, 0, 1, 1, 1, },
        { 0.5f, 1.5f, 0.5f, 0, 0, 1, 0, 1, }
    };

    Scene scene(&cam);
    Mesh3d mesh(meshVertices, { 0, 1, 2, 2, 3, 0 }, &texture);
    Mesh3d mesh2(meshVertices2, { 0, 1, 2, 2, 3, 0 }, &texture2);

    model.setup(&scene);
    mesh.setup(&scene);
    mesh2.setup(&scene);

    Terrain terrain("Heightmap.bmp", &scene);

    auto moveSlow = false;

    while (!glfwWindowShouldClose(window)) {

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }

        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
       
        mesh.updateUniforms();
        mesh.draw();

        mesh2.updateUniforms();
        mesh2.draw();

        model.updateUniforms();
        model.draw();

        terrain.draw();
        glfwSwapBuffers(window);

        //for(int i = 0; i < 1000000000; i++) {
        //    if(i % 100000000 == 0)
        //        glfwPollEvents();
        //}
        for(int i = 0; i < 100000; i++)
            if(i % 10000 == 0)
                glfwPollEvents();

        auto prevTime = time;
        time = glfwGetTime();
        auto inputs = handleInput(window, prevTime, time, cameraControl, terrain);
        glfwPollEvents();

        auto isCameraInput = [] (Input* input)
        {
            auto key = input->key;
            return key == GLFW_KEY_E || key == GLFW_KEY_S || key == GLFW_KEY_F || key == GLFW_KEY_D || key == GLFW_KEY_LEFT_SHIFT || key == GLFW_MOUSE_BUTTON_1 || key == GLFW_MOUSE_BUTTON_2;
        };
        auto isGraphicsInput = [] (Input* input)
        {
            return input->key == GLFW_KEY_Z;
        };

        for(auto input : inputs)
        {
            if(isCameraInput(input))
            {
                cameraControl.handleInput(*input);
            }
            else if(isGraphicsInput(input))
            {
                if(input->stateStart == InputType::KeyPress && input->key == GLFW_KEY_Z)
                {
                    std::cout << "hi";
                    auto mode = terrain.getDrawMode();
                    if(mode == Terrain::DrawMode::Normal)
                        terrain.setDrawMode(Terrain::DrawMode::Wireframe);
                    else if(mode == Terrain::DrawMode::Wireframe)
                        terrain.setDrawMode(Terrain::DrawMode::Flat);
                    else
                        terrain.setDrawMode(Terrain::DrawMode::Normal);
                }
            }
            delete input;
        }
    }

    model.tearDown(&scene);
    mesh.tearDown(&scene);
    mesh2.tearDown(&scene);

    glfwTerminate();
    return 0;
}
