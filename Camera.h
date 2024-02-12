#pragma once
#define NOMINMAX

#include "Vector3.h"
#include <iostream>
#include "Matrix4.h"

class Terrain;
class Input;

class Camera
{
public:
    Camera(Vector3 pos, Vector3 dir, Vector3 up, real fov, real ar);
    Matrix4 getMatrix();
    void setUp(Vector3 up);
public:
    Vector3 pos, dir, up;
    real fov, ar;
};
