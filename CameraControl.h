#pragma once

#define NOMINMAX
#include "Utils.h"
#include "Vector3.h"

class Camera;
class Terrain;
class InputQueue;
class Input;


enum CameraMode
{
    FollowingReset = 0, Freelook = 1, Following = 2
};


class CameraControl
{
    real panningX, panningY;
    real prevX = NAN, prevY = NAN; // TODO: make this a vector instead
    real terrainDist;

    CameraMode cameraMode;

    Vector3 terrainPos;

    bool moveSlow;
    real theta, phi;

    Camera* cam;
    Terrain* terrain;

    void setPosFromTerrainPos();
    void setTerrainPosFromPos();

public:
    CameraControl(Camera* cam, Terrain* terrain);

    real getPhi();
    real getTheta();

    void handleInput(const Input& input);

    Camera* getCamera();

    void setAngle(real theta, real phi);
    void moveForward(real t);
    void moveRight(real t);
    void changeMode(CameraMode);
    CameraMode getMode() const;
};
