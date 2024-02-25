#pragma once

#define NOMINMAX
#include "Utils.h"
#include "Vector3.h"

class Camera;
class Terrain;
class InputQueue;
class Input;

class CameraControl
{
    int panningX, panningY;
    real prevX = NAN, prevY = NAN; // TODO: make this a vector instead
    real terrainDist;

    Vector3 terrainPos;

    bool followingTerrain = true;
    bool panning = false;
    bool moveSlow;
    real theta, phi;

    Camera* cam;
    Terrain* terrain;

    void setPosFromTerrainPos();

public:
    CameraControl(Camera* cam, Terrain* terrain, bool followTerrain=true);

    real getPhi();
    real getTheta();

    void handleInput(const Input& input);

    Camera* getCamera();

    void setAngle(real theta, real phi);
    void moveForward(real t);
    void moveRight(real t);
    void changeMode(bool);
};
