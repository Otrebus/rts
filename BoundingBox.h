#pragma once

#include "Vector3.h"

class Ray;

class BoundingBox
{
public:
    BoundingBox(const Vector3& x, const Vector3& y);
    BoundingBox();
    ~BoundingBox();
    bool Intersect(const Ray& ray, double& tnear, double& tfar) const;
    Vector3 c1, c2;
};
