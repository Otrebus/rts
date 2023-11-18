#pragma once

#include "Vector3.h"
#include <iostream>
#include "Matrix4.h"

class Camera
{
public:
    Matrix4 GetMatrix(float fov, float ar);
public:
    Vector3 pos, dir, up;
};

class CameraControl
{
    int panningX, panningY;
    bool panning = false;
    real theta, phi;

    Camera* cam;
public:
    CameraControl(int panningX, int panningY, bool panning, real theta, real phi, Camera* cam);

    real getPhi();
    real getTheta();

    Camera* getCamera();

    void setAngle(real theta, real phi);
    void moveForward(real t);
    void moveRight(real t);
};