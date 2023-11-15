#define NOMINMAX
#include "Input.h"
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <iostream>
#include "Camera.h"
#include "Terrain.h"


InputQueue inputQueue;
bool panning;
real prevX = NAN, prevY = NAN;

auto key_callback = [] (GLFWwindow* window, int key, int scancode, int action, int mods) {
        inputQueue.addKeyInput(glfwGetTime(), key, action);
    };

auto mouseButton_callback = [] (GLFWwindow* window, int button, int action, int mods) {
    inputQueue.addMouseInput(glfwGetTime(), button, action);
};

auto cursor_position_callback = [] (GLFWwindow* window, double xpos, double ypos)
{         
    inputQueue.addMousePosition(glfwGetTime(), xpos, ypos);
};


void initInput(GLFWwindow* window)
{
    panning = false;
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouseButton_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
}


void handleInput(GLFWwindow* window, real prevTime, real time, CameraControl& cameraControl, Terrain& terrain) {
    
    while(inputQueue.hasInput())
    {
        auto input = inputQueue.peek();
        if(input.type == Input::Type::KeyPress)
        {
            if(input.state == GLFW_PRESS || input.state == GLFW_RELEASE) {
                std::cout << input.time << ": " << (std::string("key was ") + ((input.state == GLFW_PRESS) ? "pressed" : "released")) << std::endl;

                if(input.state == GLFW_RELEASE && input.key == GLFW_KEY_E)
                {
                    auto duration = input.time - inputQueue.timeKey[GLFW_KEY_E];
                    auto t = std::min(time - prevTime, duration);
                    inputQueue.timeKey[GLFW_KEY_E] += t;
                    std::cout << t << " " << duration << std::endl;
                    std::cout << cameraControl.getCamera()->pos.z;
                    cameraControl.moveForward(t*3);
                }
                if(input.state == GLFW_RELEASE && input.key == GLFW_KEY_D)
                {
                    auto duration = input.time - inputQueue.timeKey[GLFW_KEY_D];
                    auto t = std::min(time - prevTime, duration);
                    inputQueue.timeKey[GLFW_KEY_E] += t;
                    std::cout << "pos: " << cameraControl.getCamera()->pos.x << std::endl;
                    cameraControl.moveForward(-t*3);
                }
                if(input.state == GLFW_RELEASE && input.key == GLFW_KEY_S)
                {
                    auto duration = input.time - inputQueue.timeKey[GLFW_KEY_S];
                    std::cout << "|" << (time - prevTime) << std::endl;
                    auto t = std::min(time - prevTime, duration);
                    inputQueue.timeKey[GLFW_KEY_S] += t;
                    cameraControl.moveRight(t*3);
                }
                if(input.state == GLFW_RELEASE && input.key == GLFW_KEY_F)
                {
                    auto duration = time - inputQueue.timeKey[GLFW_KEY_F];
                    auto t = std::min(time - prevTime, duration);
                    inputQueue.timeKey[GLFW_KEY_F] += t;
                    cameraControl.moveRight(-t*3);
                }
                if(input.key == GLFW_KEY_Z && input.state == GLFW_PRESS)
                {
                    if(terrain.GetDrawMode() == Terrain::DrawMode::Normal)
                        terrain.SetDrawMode(Terrain::DrawMode::Wireframe);
                    else
                        terrain.SetDrawMode(Terrain::DrawMode::Normal);
                }
            }
        }

        if(!panning && inputQueue.mouseState[GLFW_MOUSE_BUTTON_1]) {
            glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            
            prevX = inputQueue.posX;
            prevY = inputQueue.posY;
            panning = true;
        }

        if(panning) {
            if(inputQueue.mouseState[GLFW_MOUSE_BUTTON_1] == GLFW_RELEASE) {
                panning = false;
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }

            std::cout << inputQueue.posX - prevX << std::endl;
            if(!isnan(prevX)) {
                cameraControl.setAngle(
                    cameraControl.getTheta() + (inputQueue.posX - prevX)/500.0,
                    cameraControl.getPhi() + (inputQueue.posY - prevY)/500.0
                );
            }
            prevX = inputQueue.posX;
            prevY = inputQueue.posY;
        }
        inputQueue.pop();
        
        //std::cout << cameraControl.getCamera()->dir.x << std::endl;

    }

    if(inputQueue.keyState[GLFW_KEY_E] == GLFW_PRESS)
    {
        auto duration = inputQueue.timeKey[GLFW_KEY_E];
        auto t = std::min(time - prevTime, duration);
        inputQueue.timeKey[GLFW_KEY_E] += t;
        cameraControl.moveForward(t*3);
    }
    if(inputQueue.keyState[GLFW_KEY_D] == GLFW_PRESS)
    {
        auto duration = inputQueue.timeKey[GLFW_KEY_D];
        auto t = std::min(time - prevTime, duration);
        inputQueue.timeKey[GLFW_KEY_D] += t;
        cameraControl.moveForward(-t*3);
    }
    if(inputQueue.keyState[GLFW_KEY_S] == GLFW_PRESS)
    {
        auto duration = inputQueue.timeKey[GLFW_KEY_S];
        auto t = std::min(time - prevTime, duration);
        inputQueue.timeKey[GLFW_KEY_S] += t;
        cameraControl.moveRight(-t*3);
    }
    if(inputQueue.keyState[GLFW_KEY_F] == GLFW_PRESS)
    {
        auto duration = inputQueue.timeKey[GLFW_KEY_F];
        auto t = std::min(time - prevTime, duration);
        inputQueue.timeKey[GLFW_KEY_F] += t;
        cameraControl.moveRight(t*3);
    }
}
