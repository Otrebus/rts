#pragma once

#include "Utils.h"
#include "Vector3.h"

class Ray;

class BoundingBox
{
public:
    BoundingBox(const Vector3& x, const Vector3& y);
    BoundingBox();
    ~BoundingBox();
    bool intersect(const Ray& ray, real& tnear, real& tfar) const;
    Vector3 getNormal(const Ray& ray, real& tnear, real& tfar) const;

    Vector3 c1, c2;
};
