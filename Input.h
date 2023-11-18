#include <Queue>
#include "Utils.h"
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include "Camera.h"
#include "Terrain.h"


void initInput(GLFWwindow* window);
void handleInput(GLFWwindow* window, real prevTime, real time, CameraControl& cameraControl, Terrain& terrain);


struct Input
{
    real time;
    enum Type
    {
        MousePress, KeyPress, MousePosition
    } type;
    int key, state;
    real posX, posY;
};

struct InputQueue
{
    std::queue<Input> queue;
    bool mouseState[8];
    bool keyState[255];
    real posX, posY;

    real timeMouse[8];
    real timeKey[255];

    InputQueue();

    void addKeyInput(real time, int key, int state);
    void addMouseInput(real time, int key, int state);
    void addMousePosition(real time, real x, real y);
    bool hasInput();
    Input peek();
    Input pop();
};
