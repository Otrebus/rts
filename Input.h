#pragma once
#include <Queue>
#include "Utils.h"
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include "Camera.h"
#include "Terrain.h"

enum InputType
{
    MouseRelease = 1, MousePress = 2, MouseHold = 4, KeyRelease = 8, KeyPress = 16, KeyHold = 32, MousePosition = 64
};


struct QueuedInput
{
    real time;
    InputType type;
    int key, state;
    real posX, posY;
};

struct InputQueue
{
    std::queue<QueuedInput> queue;
    bool mouseState[8];
    bool keyState[GLFW_KEY_LAST];
    real posX, posY;

    real timeMouse[8];
    real timeKey[GLFW_KEY_LAST];

    InputQueue();

    void addKeyInput(real time, int key, int state);
    void addMouseInput(real time, int key, int state);
    void addMousePosition(real time, real x, real y);
    bool hasInput();
    QueuedInput peek();
    QueuedInput pop();
};

struct Input
{
    Input() {}
    Input(real posX, real posY, int key, InputType stateStart, InputType stateEnd, real timeStart, real timeEnd) : posX(posX), posY(posY), key(key), stateStart(stateStart), stateEnd(stateEnd), timeStart(timeStart), timeEnd(timeEnd) {}
    InputType type;
    real posX, posY;
    int key;
    int stateStart, stateEnd;
    real timeStart, timeEnd;
};


void initInput(GLFWwindow* window);
std::vector<Input> handleInput(GLFWwindow* window, real prevTime, real time, CameraControl& cameraControl, Terrain& terrain);
