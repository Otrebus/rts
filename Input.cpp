#define NOMINMAX
#include "Input.h"
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <iostream>
#include "Camera.h"
#include "Terrain.h"
#include <unordered_map>


InputQueue::InputQueue()
{
    posX = posY = NAN;
    std::memset(mouseState, 0, sizeof(mouseState));
    std::memset(keyState, 0, sizeof(keyState));
}


void InputQueue::addKeyInput(real time, int key, int state)
{
    queue.push({ time, QueuedInput::Type::KeyPress, key, state });
}


void InputQueue::addMouseInput(real time, int key, int state)
{
    queue.push({ time, QueuedInput::Type::MousePress, key, state });
}


void InputQueue::addMousePosition(real time, real x, real y)
{
    queue.push({ time, QueuedInput::Type::MousePosition, 0, 0, x, y });
}


bool InputQueue::hasInput()
{
    return !queue.empty();
}


QueuedInput InputQueue::peek() 
{
    return queue.front();
}


QueuedInput InputQueue::pop()
{
    auto input = queue.front();
    queue.pop();
    if(input.type == QueuedInput::Type::MousePress)
    {
        timeMouse[input.key] = input.time;
        mouseState[input.key] = input.state;
    }

    if(input.type == QueuedInput::Type::KeyPress && input.state != GLFW_REPEAT)
    {
        timeKey[input.key] = input.time;
        keyState[input.key] = input.state;
    }
        
    if(input.type == QueuedInput::Type::MousePosition)
    {
        // std::cout << input.posX << " " << input.posY << std::endl;
        posX = input.posX, posY = input.posY;
    }
    return input;
}


InputQueue inputQueue;
bool panning;
real prevX = NAN, prevY = NAN;

auto key_callback = [] (GLFWwindow* window, int key, int scancode, int action, int mods)
{
    inputQueue.addKeyInput(glfwGetTime(), key, action);
};

auto mouseButton_callback = [] (GLFWwindow* window, int button, int action, int mods)
{
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


std::vector<Input> handleInput(GLFWwindow* window, real prevTime, real time, CameraControl& cameraControl, Terrain& terrain)
{    
    bool slow = false;
    if(inputQueue.keyState[GLFW_KEY_LEFT_SHIFT] == GLFW_PRESS)
        slow = true;

    std::unordered_map<int, Input*> lastInput;
    std::vector<Input*> inputs;

    while(inputQueue.hasInput())
    {
        auto queuedInput = inputQueue.peek();
        if(queuedInput.type == QueuedInput::Type::KeyPress)
        {
            inputs.push_back(new Input());
            Input* input = inputs.back();
            input->key = input->key;

            if(queuedInput.state == GLFW_RELEASE)
            {
                if(lastInput.find(queuedInput.key) == lastInput.end())
                    // A release without a press means we've held the button since last frame
                    lastInput[queuedInput.key] = new Input(0, 0, queuedInput.key, Input::Type::KeyHold, Input::Type::KeyRelease, prevTime, queuedInput.time);
                else
                {
                    // If there's any consistency at all, this previous input that we added for key must be a press or hold
                    auto prevInput = lastInput[queuedInput.key];
                    input->timeStart = inputQueue.timeKey[queuedInput.key];
                    input->timeEnd = queuedInput.time;
                    input->stateStart = Input::Type::KeyPress;

                    prevInput->stateEnd = Input::Type::KeyRelease;
                    prevInput->timeEnd = queuedInput.time;
                }
            }
            else // queuedInput.state == GLFW_PRESS
            {
                if(lastInput.find(queuedInput.key) != lastInput.end())
                {
                    auto& prevInput = lastInput[queuedInput.key];
                    if(!prevInput->stateEnd)
                    {
                        prevInput->stateEnd = Input::Type::KeyRelease;
                        prevInput->timeEnd = queuedInput.time;
                    }
                }
            }
            lastInput[queuedInput.key] = input;

            std::cout << queuedInput.time << ": " << (std::string("key was ") + ((queuedInput.state == GLFW_PRESS) ? "pressed" : "released")) << std::endl;

            if(queuedInput.state == GLFW_RELEASE && queuedInput.key == GLFW_KEY_E)
            {
                auto duration = queuedInput.time - inputQueue.timeKey[GLFW_KEY_E];
                auto t = std::min(time - prevTime, duration);
                cameraControl.moveForward(slow ? 0.03*t : t*3);
            }
            if(queuedInput.state == GLFW_RELEASE && queuedInput.key == GLFW_KEY_D)
            {
                auto duration = queuedInput.time - inputQueue.timeKey[GLFW_KEY_D];
                auto t = std::min(time - prevTime, duration);
                cameraControl.moveForward(slow ? -0.03*t : -t*3);
            }
            if(queuedInput.state == GLFW_RELEASE && queuedInput.key == GLFW_KEY_S)
            {
                auto duration = queuedInput.time - inputQueue.timeKey[GLFW_KEY_S];
                auto t = std::min(time - prevTime, duration);
                cameraControl.moveRight(slow ? -0.03*t : -t*3);
            }
            if(queuedInput.state == GLFW_RELEASE && queuedInput.key == GLFW_KEY_F)
            {
                auto duration = queuedInput.time - inputQueue.timeKey[GLFW_KEY_F];
                auto t = std::min(time - prevTime, duration);
                cameraControl.moveRight(slow ? 0.03*t : t*3);
            }
            if(queuedInput.key == GLFW_KEY_Z && queuedInput.state == GLFW_PRESS)
            {
                auto mode = terrain.GetDrawMode();
                if(mode == Terrain::DrawMode::Normal)
                    terrain.SetDrawMode(Terrain::DrawMode::Wireframe);
                else if(mode == Terrain::DrawMode::Wireframe)
                    terrain.SetDrawMode(Terrain::DrawMode::Flat);
                else
                    terrain.SetDrawMode(Terrain::DrawMode::Normal);
            }
            inputs.push_back(input);
        }

        if(!panning && inputQueue.mouseState[GLFW_MOUSE_BUTTON_1])
        {
            glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            
            prevX = inputQueue.posX;
            prevY = inputQueue.posY;
            panning = true;
        }

        if(panning)
        {
            if(inputQueue.mouseState[GLFW_MOUSE_BUTTON_1] == GLFW_RELEASE)
            {
                panning = false;
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }

            std::cout << inputQueue.posX - prevX << std::endl;
            if(!isnan(prevX))
            {
                cameraControl.setAngle(
                    cameraControl.getTheta() - (inputQueue.posX - prevX)/500.0,
                    cameraControl.getPhi() - (inputQueue.posY - prevY)/500.0
                );
            }
            prevX = inputQueue.posX;
            prevY = inputQueue.posY;
        }
        inputQueue.pop();
    }

    if(inputQueue.keyState[GLFW_KEY_E] == GLFW_PRESS)
    {
        auto prevT = inputQueue.timeKey[GLFW_KEY_E];
        auto t = std::min(time - prevTime, time-prevT);
        inputQueue.timeKey[GLFW_KEY_E] = time;
        cameraControl.moveForward(slow ? t*0.03 : t*3);
    }
    if(inputQueue.keyState[GLFW_KEY_D] == GLFW_PRESS)
    {
        auto prevT = inputQueue.timeKey[GLFW_KEY_D];
        auto t = std::min(time - prevTime, time-prevT);
        inputQueue.timeKey[GLFW_KEY_D] = time;
        cameraControl.moveForward(slow ? -t*0.03 : -t*3);
    }
    if(inputQueue.keyState[GLFW_KEY_S] == GLFW_PRESS)
    {
        auto prevT = inputQueue.timeKey[GLFW_KEY_S];
        auto t = std::min(time - prevTime, time-prevT);
        inputQueue.timeKey[GLFW_KEY_S] = time;
        cameraControl.moveRight(slow ? -t*0.03 : -t*3);
    }
    if(inputQueue.keyState[GLFW_KEY_F] == GLFW_PRESS)
    {
        auto prevT = inputQueue.timeKey[GLFW_KEY_F];
        auto t = std::min(time - prevTime, time-prevT);
        std::cout << (time - prevTime) << " " << t << std::endl;
        inputQueue.timeKey[GLFW_KEY_F] = time;
        cameraControl.moveRight(slow ? t*0.03 : t*3);
    }
    return {};
}
