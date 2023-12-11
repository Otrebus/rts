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
    if(state != GLFW_REPEAT)
        queue.push({ time, InputType::KeyPress, key, state });
}


void InputQueue::addMouseInput(real time, int key, int state)
{
    queue.push({ time, InputType::MousePress, key, state });
}


void InputQueue::addMousePosition(real time, real x, real y)
{
    queue.push({ time, InputType::MousePosition, 0, 0, x, y });
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
    if(input.type == InputType::MousePress)
    {
        timeMouse[input.key] = input.time;
        mouseState[input.key] = input.state;
    }

    if(input.type == InputType::KeyPress)
    {
        timeKey[input.key] = input.time;
        keyState[input.key] = input.state;
    }
        
    if(input.type == InputType::MousePosition)
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

        if(queuedInput.type == KeyPress || queuedInput.type == KeyHold || queuedInput.type == KeyRelease)
        {
            if(auto it = lastInput.find(queuedInput.key); it != lastInput.end())
            {
                auto prevInput = it->second;
                if(queuedInput.type == InputType::KeyPress)
                {
                    if(prevInput->stateEnd == InputType::KeyHold || !prevInput->stateEnd)
                        __debugbreak(); // Shouldn't happen
                    else
                    {
                        // A keypress after a release, start a new input
                        Input* input = new Input();
                        inputs.push_back(input);
                        input->key = queuedInput.key;
                        input->timeStart = queuedInput.time;
                        input->stateStart = queuedInput.type;
                    }
                }
                else if(queuedInput.type == InputType::KeyRelease)
                {
                    if(!prevInput->stateEnd || prevInput->stateEnd == InputType::KeyHold)
                    {
                        // A release after a hold or press, the input will be extended
                        prevInput->stateEnd = queuedInput.type;
                        prevInput->timeEnd = queuedInput.time;
                    }
                    else
                        __debugbreak();
                }
                else
                    __debugbreak(); // Shouldn't happen
            }
            else
            {
                auto prevInput = inputQueue.keyState[queuedInput.key];
                if(queuedInput.type == InputType::KeyPress)
                {
                    if(prevInput)
                        __debugbreak(); // Shouldn't happen
                    else
                    {
                        // A new input after a release
                        Input* input = new Input();
                        inputs.push_back(input);
                        input->key = queuedInput.key;
                        input->timeStart = inputQueue.timeKey[queuedInput.key];
                        input->stateStart = input->type;
                    }
                }
                // todoododo
                /*prevInput->stateEnd = InputType::KeyRelease;
                prevInput->timeEnd = queuedInput.time;*/
                //lastInput[queuedInput.key] = input
                //lastInput[queuedInput.key] = new Input(0, 0, queuedInput.key, InputType::KeyHold, InputType::KeyRelease, prevTime, queuedInput.time);
            }
            for(int key = 0; key < GLFW_KEY_LAST; key++)
            {
                if(inputQueue.keyState[key] == GLFW_PRESS && lastInput.find(key) == lastInput.end())
                {
                    Input* input = new Input();
                    inputs.push_back(input);
                    input->key = queuedInput.key;
                    input->timeStart = prevTime;
                    input->timeEnd = time;
                    input->stateStart = InputType::KeyHold;
                    input->stateEnd = InputType::KeyHold;
                }
            }

            if(queuedInput.type == InputType::KeyPress)
            {
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
            }
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
