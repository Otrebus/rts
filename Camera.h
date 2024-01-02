#pragma once

#include "Vector3.h"
#include <iostream>
#include "Matrix4.h"

class Input;

class Camera
{
public:
    Matrix4 getMatrix(float fov, float ar);
public:
    Vector3 pos, dir, up;
};

class InputQueue;

class CameraControl
{
    int panningX, panningY;
    real prevX = NAN, prevY = NAN;

    bool panning = false;
    bool moveSlow;
    real theta, phi;

    Camera* cam;
public:
    CameraControl(int panningX, int panningY, bool panning, real theta, real phi, Camera* cam);

    real getPhi();
    real getTheta();

    void handleInput(const Input& input);

    Camera* getCamera();

    void setAngle(real theta, real phi);
    void moveForward(real t);
    void moveRight(real t);
};