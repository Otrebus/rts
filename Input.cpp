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
    for(int i = 0; i < GLFW_KEY_END; i++)
        keyState[i] = GLFW_RELEASE;

    for(int i = 0; i < GLFW_MOUSE_BUTTON_LAST; i++)
        mouseState[i] = GLFW_RELEASE;
}


void InputQueue::addKeyInput(real time, int key, int state)
{
    if(state != GLFW_REPEAT)
        queue.push({ time, QueuedInputType::KeyboardKey, key, state, 0, 0, this });
}


void InputQueue::addMouseInput(real time, int key, int state)
{
    queue.push({ time, QueuedInputType::MouseButton, key, state, 0, 0, this });
}


void InputQueue::addMousePosition(real time, real x, real y)
{
    queue.push({ time, QueuedInputType::MousePos, 0, 0, x, y, this });
}

void InputQueue::addScrollOffset(real time, real y)
{
    queue.push({ time, QueuedInputType::Scroll, 0, 0, 0, y, this });
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
    if(input.type == QueuedInputType::MouseButton)
    {
        timeMouse[input.key] = input.time;
        mouseState[input.key] = input.state;
        lastMouseKey[input.key] = Input(0, 0, input.key, input.state ? MousePress : MouseRelease, None, input.time, 0.0);
    }

    if(input.type == QueuedInputType::KeyboardKey)
    {
        timeKey[input.key] = input.time;
        keyState[input.key] = input.state;
        lastKeyboardKey[input.key] = Input(0, 0, input.key, input.state ? KeyPress : KeyRelease, None, input.time, 0.0);
    }
        
    if(input.type == QueuedInputType::MousePos)
        posX = input.posX, posY = input.posY;
    return input;
}

bool panning;

auto keyCallback = [] (GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(action != GLFW_REPEAT)
        InputQueue::getInstance().addKeyInput(glfwGetTime(), key, action);
};

auto mouseButtonCallback = [] (GLFWwindow* window, int button, int action, int mods)
{
    InputQueue::getInstance().addMouseInput(glfwGetTime(), button, action);
};

auto cursorPositionCallback = [] (GLFWwindow* window, double xpos, double ypos)
{         
    InputQueue::getInstance().addMousePosition(glfwGetTime(), xpos, ypos);
};

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    InputQueue::getInstance().addScrollOffset(glfwGetTime(), yoffset);
}


void InputQueue::initInput(GLFWwindow* window)
{
    panning = false;
    InputQueue::getInstance().setWindow(window);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPositionCallback);
    glfwSetScrollCallback(window, scrollCallback);
}

void InputQueue::captureMouse(bool capture)
{
    if(capture)
    {
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    else
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

bool InputQueue::isKeyHeld(int key) const
{
    return keyState[key];
}

void InputQueue::setWindow(GLFWwindow* window)
{
    this->window = window;
}


std::vector<Input*> InputQueue::handleInput(real prevTime, real time)
{
    bool slow = false;
    if(keyState[GLFW_KEY_LEFT_SHIFT] == GLFW_PRESS)
        slow = true;

    std::unordered_map<int, Input*> lastKeyInput;
    std::unordered_map<int, Input*> lastMouseInput;
    std::vector<Input*> inputs;

    auto pushInput = [&inputs, this] (std::unordered_map<int, Input*>& lastInput, int key, real time, InputType start, InputType end)
    {
        Input* input = new Input();
        inputs.push_back(input);
        lastInput[key] = input;
        input->key = key;
        input->timeStart = time;
        input->stateStart = start;
        input->stateEnd = end;
        return input;
    };

    while(hasInput())
    {
        auto queuedInput = peek();
        if(queuedInput.type == KeyboardKey || queuedInput.type == MouseButton)
        {
            auto &lastInput = (queuedInput.type == KeyboardKey ? lastKeyInput : lastMouseInput);
            auto Press = (queuedInput.type == KeyboardKey ? KeyPress : MousePress);
            auto Release = (queuedInput.type == KeyboardKey ? KeyRelease : MouseRelease);
            auto Hold = (queuedInput.type == KeyboardKey ? KeyHold : MouseHold);

            // Check whether we have added an input of this type to the queue already
            if(auto it = lastInput.find(queuedInput.key); it != lastInput.end())
            {
                // This key was added (the user is spamming the key, or the fps is very low)
                auto prevInput = it->second;

                // Check if the key is on its way in or out
                if(queuedInput.state == GLFW_PRESS)
                    // Here we just assume that the previous event of the key must have been a release
                    pushInput(lastInput, queuedInput.key, queuedInput.time, Press, None);
                else if(queuedInput.state == GLFW_RELEASE)
                    // Here we assume that the previous event was a press, so we extend it into a press/release (ha)
                    prevInput->stateEnd = Release, prevInput->timeEnd = queuedInput.time;
            }
            else
            {
                // No previous action of this key has been recorded yet
                if(queuedInput.state == GLFW_PRESS)
                    // Key was not active, this is a brand new press, we assume
                    pushInput(lastInput, queuedInput.key, queuedInput.time, Press, None);
                else
                    pushInput(lastInput, queuedInput.key, prevTime, Hold, Release)->timeEnd = queuedInput.time;
            }
        }
        else if(queuedInput.type == MousePos)
            inputs.push_back(new Input(queuedInput.posX, queuedInput.posY, 0, InputType::MousePosition, None, queuedInput.time, queuedInput.time));
        else if(queuedInput.type == Scroll)
            inputs.push_back(new Input(queuedInput.posX, queuedInput.posY, 0, InputType::ScrollOffset, None, queuedInput.time, queuedInput.time));
        pop();
    }

    for(int key = 0; key < GLFW_KEY_LAST; key++)
    {
        if(keyState[key] == GLFW_PRESS)
        {
            if(lastKeyInput.find(key) == lastKeyInput.end())
                pushInput(lastKeyInput, key, timeKey[key], KeyHold, KeyHold)->timeEnd = time;
            else
            {
                auto prevInput = lastKeyInput[key];
                if(prevInput->stateEnd || prevInput->stateStart != InputType::KeyPress)
                    __debugbreak();
                prevInput->stateEnd = InputType::KeyHold;
                prevInput->timeEnd = time;
            }
            timeKey[key] = time;
        }
    }

    for(int button = 0; button < GLFW_MOUSE_BUTTON_LAST; button++)
    {
        if(mouseState[button] == GLFW_PRESS)
        {
            if(lastMouseInput.find(button) == lastMouseInput.end())
                pushInput(lastMouseInput, button, timeMouse[button], MouseHold, MouseHold)->timeEnd = time;
            else
            {
                auto prevInput = lastMouseInput[button];
                if(prevInput->stateEnd || prevInput->stateStart != InputType::MousePress)
                    __debugbreak();
                prevInput->stateEnd = InputType::MouseHold;
                prevInput->timeEnd = time;
            }
            timeMouse[button] = time;
        }
    }
    return inputs;
}
