#pragma once

#define NOMINMAX
#include "Line.h"
#include "Ray.h"
#include "Vector2.h"

class Input;
class Scene;
class Entity;
class Ray;
class CameraControl;
class Unit;

enum SelectState
{
    NotSelecting, Clicking, DrawingBox
};

enum CommandState
{
    NotCommanding, Moving, Attacking
};

class UserInterface
{
public:
    UserInterface(GLFWwindow* window, Scene* scene, CameraControl* cameraControl);

    void draw();

    void handleInput(const Input& input, const std::vector<Unit*>& units);
    void setResolution(int xres, int yres);

    void selectUnits(std::vector<Unit*> units, bool pre);
    void selectUnit(const Ray& ray, const std::vector<Unit*>& units, bool pre);

    Unit* getUnit(const Ray& ray, const std::vector<Unit*>& units) const;

    void setCursor(int shape);

    void moveDrawnUnits(const std::vector<Unit*>& selectedUnits);

//private:
    CameraControl* cameraControl;
    int xres, yres;
    int mouseX, mouseY;
    enum SelectState selectState;
    enum CommandState commandState;
    Vector2 drawBoxc1, drawBoxc2;
    Scene* scene;

    bool selectingAdditional;
    bool intersecting;
    Unit* movingUnit;

    std::vector<Vector3> drawTarget;

    Ray intersectRay;
    GLFWcursor* cursor;
    GLFWwindow* window;
};
