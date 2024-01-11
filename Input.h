#pragma once
#include <Queue>
#include "Utils.h"
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include "Terrain.h"
#include "CameraControl.h"

class InputQueue;

enum InputType
{
    None = 0, MouseRelease = 1, MousePress = 2, MouseHold = 4, KeyRelease = 8, KeyPress = 16, KeyHold = 32, MousePosition = 64, ScrollOffset = 128
};

enum QueuedInputType
{
    KeyboardKey = 1, MouseButton = 2, MousePos = 4, Scroll = 8
};


struct QueuedInput
{
    real time;
    QueuedInputType type;
    int key, state;
    real posX, posY;
    InputQueue* inputQueue;
};

struct Input
{
    Input() {}
    Input(real posX, real posY, int key, InputType stateStart, InputType stateEnd, real timeStart, real timeEnd, InputQueue* queue) : posX(posX), posY(posY), key(key), stateStart(stateStart), stateEnd(stateEnd), timeStart(timeStart), timeEnd(timeEnd), inputQueue(queue) {}
    real posX, posY;
    int key;
    InputType stateStart, stateEnd;
    real timeStart, timeEnd;
    InputQueue* inputQueue;
};

struct InputQueue
{
    Input lastKeyboardKey[GLFW_KEY_LAST], lastMouseKey[GLFW_MOUSE_BUTTON_LAST];

    std::queue<QueuedInput> queue;
    bool mouseState[8];
    int keyState[GLFW_KEY_LAST];
    real posX, posY;
    GLFWwindow* window;

    real timeMouse[8];
    real timeKey[GLFW_KEY_LAST];

    InputQueue();

    void addKeyInput(real time, int key, int state);
    void addMouseInput(real time, int key, int state);
    void addMousePosition(real time, real x, real y);
    void addScrollOffset(real time, real y);

    void setWindow(GLFWwindow* window);
    void captureMouse(bool);

    bool hasInput();
    QueuedInput peek();
    QueuedInput pop();
};


void initInput(GLFWwindow* window);
std::vector<Input*> handleInput(GLFWwindow* window, real prevTime, real time, CameraControl& cameraControl, Terrain& terrain);
