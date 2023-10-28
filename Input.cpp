#define NOMINMAX
#include "Input.h"
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <iostream>
#include "Camera.h"


InputQueue inputQueue;
bool panning;
real prevX, prevY;

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


void handleInput(GLFWwindow* window, real prevTime, real time, CameraControl& cameraControl) {

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
                    cameraControl.moveForward(t*3);
                }
                if(input.state == GLFW_RELEASE && input.key == GLFW_KEY_D)
                {
                    auto duration = input.time - inputQueue.timeKey[GLFW_KEY_D];
                    auto t = std::min(time - prevTime, duration);
                    cameraControl.moveForward(-t*3);
                }
                if(input.state == GLFW_RELEASE && input.key == GLFW_KEY_S)
                {
                    auto duration = input.time - inputQueue.timeKey[GLFW_KEY_S];
                    auto t = std::min(time - prevTime, duration);
                    cameraControl.moveRight(t*3);
                }
                if(input.state == GLFW_RELEASE && input.key == GLFW_KEY_F)
                {
                    auto duration = time - inputQueue.timeKey[GLFW_KEY_F];
                    auto t = std::min(time - prevTime, duration);
                    cameraControl.moveRight(-t*3);
                }
            }
        }

        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        inputQueue.posX = xpos;
        inputQueue.posY = ypos;


        if(!panning && inputQueue.mouseState[GLFW_MOUSE_BUTTON_1]) {
            glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

            panning = true;
        }

        if(panning && inputQueue.mouseState[GLFW_MOUSE_BUTTON_1] == GLFW_RELEASE) {
            panning = false;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }


        if(panning) {
            std :: cout <<inputQueue.posX - prevX << std::endl;
            cameraControl.setAngle(
                cameraControl.getTheta() + (inputQueue.posX - prevX)/500,
                cameraControl.getPhi() + (inputQueue.posY - prevY)/500
            );
        }
        inputQueue.pop();
        
        std::cout << cameraControl.getCamera()->dir.x << std::endl;

        prevX = inputQueue.posX;
        prevY = inputQueue.posY;
    }

    if(inputQueue.keyState[GLFW_KEY_E] == GLFW_PRESS)
    {
        auto duration = inputQueue.timeKey[GLFW_KEY_E];
        auto t = std::min(time - prevTime, duration);
        cameraControl.moveForward(t*3);
    }
    if(inputQueue.keyState[GLFW_KEY_D] == GLFW_PRESS)
    {
        auto duration = inputQueue.timeKey[GLFW_KEY_D];
        auto t = std::min(time - prevTime, duration);
        cameraControl.moveForward(-t*3);
    }
    if(inputQueue.keyState[GLFW_KEY_S] == GLFW_PRESS)
    {
        auto duration = inputQueue.timeKey[GLFW_KEY_S];
        auto t = std::min(time - prevTime, duration);
        cameraControl.moveRight(t*3);
    }
    if(inputQueue.keyState[GLFW_KEY_F] == GLFW_PRESS)
    {
        auto duration = inputQueue.timeKey[GLFW_KEY_F];
        auto t = std::min(time - prevTime, duration);
        cameraControl.moveRight(-t*3);
    }
}
