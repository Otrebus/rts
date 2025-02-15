#pragma once
#define NOMINMAX

#include "Matrix4.h"
#include "Vector3.h"

class Terrain;
class Input;
class Ray;

class Camera
{
public:
    Camera();
    Camera(Vector3 pos, Vector3 dir, Vector3 up, real ar);

    virtual Matrix4 getMatrix() = 0;
    virtual Matrix4 getScreenMatrix() = 0;
    void setUp(Vector3 up);

    void setPos(const Vector3& pos);
    void setDir(const Vector3& dir);

    const Vector3& getPos() const;
    const Vector3& getDir() const;
    const Vector3& getUp() const;
    const real getAspectRatio() const;

    virtual Ray getViewRay(real x, real y) const = 0;

protected:
    Matrix4 viewMatrix;
    bool matrixCached;

    Vector3 pos, dir, up;
    real ar;
};


class PerspectiveCamera : public Camera
{
public:
    PerspectiveCamera(Vector3 pos, Vector3 dir, Vector3 up, real fov, real ar);
    Ray getViewRay(real x, real y) const;
    Matrix4 getMatrix();
    Matrix4 getScreenMatrix();

    const real getFov() const;

    real fov;
};

class OrthogonalCamera : public Camera
{
public:
    OrthogonalCamera(Vector3 pos, Vector3 dir, Vector3 up, real ar);
    Ray getViewRay(real x, real y) const;
    Matrix4 getMatrix();
    Matrix4 getScreenMatrix();
};