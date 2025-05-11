#pragma once

#define NOMINMAX
#include "Utils.h"
#include "Vector3.h"
#include "Vector2.h"

class Camera;
class Terrain;
class InputManager;
class Input;


enum CameraMode
{
    FollowingReset = 0, Freelook = 1, Following = 2
};



struct MovementImpulse
{
    MovementImpulse(real start, real duration, Vector3 dir) : start(start), duration(duration), dir(dir) {}

    Vector3 dir;
    real start, duration;
    bool finished;

    Vector3 getVal(real time)
    {
        auto t = (time-start)/duration;
        if(t >= 1)
            t = 1;
        return (t*(t-2)+1)*dir;
    }

    bool isFinished(real time)
    {
        auto t = (time-start)/duration;
        return t >= 1;
    }
};


class CameraControl
{
    real prevX = NAN, prevY = NAN; // TODO: make this a vector instead
    real terrainDist;
    int moveDir[4];

    CameraMode cameraMode;

    Vector3 terrainPos;

    bool moveSlow;
    real theta, phi;
    int xres, yres;

    Camera* cam;
    Terrain* terrain;

    std::vector<MovementImpulse> movementImpulses;

    void setPosFromTerrainPos();
    void setTerrainPosFromPos();
    Vector3 getPosFromTerrainPos(Vector3 v);

public:
    CameraControl(Camera* cam, Terrain* terrain, int xres, int yres);

    void setResolution(int xres, int yres);
    real getPhi();
    real getTheta();

    void handleInput(const Input& input);

    Camera* getCamera();

    void update(real dt);

    void setAngle(real theta, real phi);
    void move(Vector2 dir, real t);
    void changeMode(CameraMode);
    CameraMode getMode() const;
};
