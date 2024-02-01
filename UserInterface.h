#pragma once

#include "Vector2.h"

class Input;
class Scene;

class UserInterface
{
public:
    UserInterface(Scene* scene);

    void draw();

    void handleInput(const Input& input);
    void setResolution(int xres, int yres);

private:
    int xres, yres;
    int mouseX, mouseY;
    bool drawingBox;
    Vector2 drawBoxc1, drawBoxc2;
    Scene* scene;
};