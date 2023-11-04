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
    CameraControl(int panningX, int panningY, bool panning, real theta, real phi, Camera* cam) :
        panningX(panningX), panningY(panningY), panning(panning), theta(theta), phi(phi), cam(cam)
    {
        setAngle(theta, phi);
    }

    real getPhi() { return phi; }
    real getTheta() { return theta; }

    Camera* getCamera() { return cam; }

    void setAngle(real theta, real phi) {
        Vector3 f(0, 0, 1), r(1, 0, 0);

        this->theta = theta;
        this->phi = phi;

        cam->up = Vector3 (0, 1, 0);
        Vector3 h = f*std::sin(theta) + r*std::cos(theta);
        cam->dir = cam->up*std::sin(phi) + h*std::cos(phi);
    }

    void moveForward(real t) {
        cam->pos = cam->pos + cam->dir*t*3;
    }

    void moveRight(real t) {
        cam->pos = cam->pos - (cam->up^cam->dir)*t*3;
    }
};