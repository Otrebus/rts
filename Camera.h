#pragma once
#define NOMINMAX

#include "Vector3.h"
#include <iostream>
#include "Matrix4.h"

class Terrain;
class Input;
class Ray;

class Camera
{
public:
    Camera(Vector3 pos, Vector3 dir, Vector3 up, real fov, real ar, bool debug=false);
    Matrix4 getMatrix();
    void setUp(Vector3 up);

    void setPos(const Vector3& pos);
    void setDir(const Vector3& dir);

    const Vector3& getPos() const;
    const Vector3& getDir() const;
    const Vector3& getUp() const;
    const real getFov() const;
    const real getAspectRatio() const;

    Ray getViewRay(real x, real y) const;

private:
    Matrix4 viewMatrix;
    bool matrixCached;
    bool debug;

    Vector3 pos, dir, up;
    real fov, ar;
};
