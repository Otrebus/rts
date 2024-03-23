#pragma once

#define NOMINMAX
#include "Vector2.h"
#include "Line.h"
#include "Ray.h"

class Input;
class Scene;
class Entity;
class Ray;
class CameraControl;

enum SelectState {
    NotSelecting, Clicking, DrawingBox
};

class UserInterface
{
public:
    UserInterface(GLFWwindow* window, Scene* scene, CameraControl* cameraControl);

    void draw();

    void handleInput(const Input& input, const std::vector<Entity*>& entities);
    void setResolution(int xres, int yres);

    void selectEntities(std::vector<Entity*> entities);
    void selectEntity(const Ray& ray, const std::vector<Entity*>& entities);

    Entity* getEntity(const Ray& ray, const std::vector<Entity*>& entities) const;

    void setCursor(int shape);

//private:
    CameraControl* cameraControl;
    int xres, yres;
    int mouseX, mouseY;
    enum SelectState selectState;
    Vector2 drawBoxc1, drawBoxc2;
    Scene* scene;

    bool intersecting;
    Entity* movingEntity;

    Ray intersectRay;
    GLFWcursor* cursor;
    GLFWwindow* window;
};
