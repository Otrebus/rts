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
class Unit;

enum SelectState {
    NotSelecting, Clicking, DrawingBox
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

//private:
    CameraControl* cameraControl;
    int xres, yres;
    int mouseX, mouseY;
    enum SelectState selectState;
    Vector2 drawBoxc1, drawBoxc2;
    Scene* scene;

    bool selectingAdditional;
    bool intersecting;
    Unit* movingUnit;

    Ray intersectRay;
    GLFWcursor* cursor;
    GLFWwindow* window;
};
