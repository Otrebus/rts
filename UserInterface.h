#pragma once

#define NOMINMAX
#include "Vector2.h"
#include "Line.h"

class Input;
class Scene;
class Entity;
class Ray;

enum SelectState {
    NotSelecting, Clicking, DrawingBox
};

class UserInterface
{
public:
    UserInterface(Scene* scene);

    void draw();

    void handleInput(const Input& input, std::vector<Entity*> entities);
    void setResolution(int xres, int yres);

    void selectEntities(std::vector<Entity*> entities);
    void selectEntity(const Ray& ray, std::vector<Entity*> entities);

    Vector3 getTarget();

//private:
    int xres, yres;
    int mouseX, mouseY;
    enum SelectState selectState;
    Vector2 drawBoxc1, drawBoxc2;
    Scene* scene;
    Entity* target; // tmp
};
