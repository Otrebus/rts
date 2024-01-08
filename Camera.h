#pragma once

#include "Vector3.h"
#include <iostream>
#include "Matrix4.h"

class Terrain;
class Input;

class Camera
{
public:
    Matrix4 getMatrix(float fov, float ar);
public:
    Vector3 pos, dir, up;
};
