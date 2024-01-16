#pragma once

#include "Vector3.h"

class IntersectionInfo;

class Ray
{
public:
    Ray();
    Ray(const Vector3&, const Vector3&);
    ~Ray();

    Vector3 pos, dir;
};
