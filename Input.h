#include <Queue>
#include "Utils.h"
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include "Camera.h"


void initInput(GLFWwindow* window);
void handleInput(GLFWwindow* window, real prevTime, real time, CameraControl& cameraControl);


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

    InputQueue()
    {
        posX = posY = NAN;
        std::memset(mouseState, 0, sizeof(mouseState));
        std::memset(keyState, 0, sizeof(keyState));
    }

    void addKeyInput(real time, int key, int state)
    {
        queue.push({ time, Input::Type::KeyPress, key, state });
    }

    void addMouseInput(real time, int key, int state)
    {
        queue.push({ time, Input::Type::MousePress, key, state });
    }

    void addMousePosition(real time, real x, real y)
    {
        queue.push({ time, Input::Type::MousePosition, 0, 0, x, y });
    }

    bool hasInput()
    {
        return !queue.empty();
    }

    Input peek()
    {
        return queue.front();
    }

    Input pop()
    {
        auto input = queue.front();
        queue.pop();
        if(input.type == Input::Type::MousePress) {
            timeMouse[input.key] = input.time;
            mouseState[input.key] = input.state;
        }

        if(input.type == Input::Type::KeyPress) {
            timeKey[input.key] = input.time;
            keyState[input.key] = input.state;
        }
        
        if(input.type == Input::Type::MousePosition) {
            // std::cout << input.posX << " " << input.posY << std::endl;
            posX = input.posX, posY = input.posY;
        }
        return input;
    }
};
