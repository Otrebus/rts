#pragma once

#include "Vector2.h"
#include "Line.h"

class Input;
class Scene;
class Entity;

class UserInterface
{
public:
    UserInterface(Scene* scene);

    void draw();

    void handleInput(const Input& input, std::vector<Entity*> entities);
    void setResolution(int xres, int yres);

    void selectEntities(std::vector<Entity*> entities);

//private:
    int xres, yres;
    int mouseX, mouseY;
    bool drawingBox;
    Vector2 drawBoxc1, drawBoxc2;
    Scene* scene;

    Line3d line1, line2, line3, line4;

};
