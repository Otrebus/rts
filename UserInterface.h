#pragma once

#include "Ray.h"
#include "Console.h"
#include "Font.h"

class SelectionMarkerMesh;
class BuildingPlacerMesh;
class Input;
class Scene;
class Entity;
class CameraControl;
class Unit;

enum SelectState
{
    NotSelecting, Clicking, DrawingBox, PickingCircleCenter, DrawingCircle
};

enum CommandState
{
    NotCommanding, Moving, Attacking
};

enum BuildingPlacingState
{
    NotPlacingBuilding, PlacingBuilding
};

class UserInterface
{
public:
    UserInterface(GLFWwindow* window, Scene* scene, CameraControl* cameraControl);

    void draw();

    bool handleInput(const Input& input, const std::vector<Unit*>& units);
    void setResolution(int xres, int yres);

    void selectUnits(std::vector<Unit*> units, bool pre);
    void selectUnit(const Ray& ray, const std::vector<Unit*>& units, bool pre);

    bool isInFrustum(Unit*);

    Unit* getUnit(const Ray& ray, const std::vector<Unit*>& units) const;

    void setCursor(int shape);

    void update(real dt);

    void moveDrawnUnits(const std::vector<Unit*>& selectedUnits);

//private:
    CameraControl* cameraControl;
    int xres, yres;
    int mouseX, mouseY;
    enum SelectState selectState;
    enum CommandState commandState;
    enum BuildingPlacingState buildingPlacingState;
    Vector2 drawBoxc1, drawBoxc2;
    Scene* scene;

    Console* console;

    bool selectingAdditional;
    bool intersecting;
    bool showConsole;
    int movingUnit;

    std::vector<Vector3> drawTarget;

    real timeClickedUnit;
    Unit* lastClickedUnit;
    Material* consoleMaterial;

    BuildingPlacerMesh* buildingPlacerMesh;
    SelectionMarkerMesh* selectionMesh;

    Ray intersectRay;
    GLFWcursor* cursor;
    GLFWwindow* window;

    Font* font;

    real fps;
    Vector2 mouseGeoCoords;
    Vector2 circleCenter;

    static ConsoleVariable showFps;
    static ConsoleVariable showCoordinates;
};

void addUnitPathfindingRequest(Unit* unit, Vector2 pos);
