#pragma once

#include "Ray.h"

class Input;
class Scene;
class Entity;
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

    bool isInFrustum(Unit*);

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
    bool console;
    int movingUnit;

    std::vector<Vector3> drawTarget;

    real timeClickedUnit;
    Unit* lastClickedUnit;
    Material* consoleMaterial;

    Ray intersectRay;
    GLFWcursor* cursor;
    GLFWwindow* window;
};
