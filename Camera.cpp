#include "Camera.h"
#include "Input.h"
#include "Ray.h"


Camera::Camera(Vector3 pos, Vector3 dir, Vector3 up, real ar) : pos(pos), dir(dir.normalized()), up(up), ar(ar)
{
    matrixCached = false;
}


PerspectiveCamera::PerspectiveCamera(Vector3 pos, Vector3 dir, Vector3 up, real fov=90, real ar=1) : Camera(pos, dir.normalized(), up, ar), fov(fov)
{
    setUp(up);
}

OrthogonalCamera::OrthogonalCamera(Vector3 pos, Vector3 dir, Vector3 up, real ar=1) : Camera(pos, dir.normalized(), up, ar)
{
    this->ar = ar;
}


void Camera::setUp(Vector3 up)
{
    this->up = ((dir%up)%dir).normalized();
}

Matrix4 PerspectiveCamera::getMatrix()
{
    if(matrixCached)
        return viewMatrix;
    float pi = 3.141592653589793;
    float a = std::tan(deg(fov/2));
    float n = -0.3, f = -500.0;

    Matrix4 trans(
        1, 0, 0, -pos.x,
        0, 1, 0, -pos.y,
        0, 0, 1, -pos.z,
        0, 0, 0, 1
    );

    Vector3 d = -dir.normalized();
    Vector3 u1 = this->up;
    Vector3 v = (u1%d).normalized();
    Vector3 u = (d%v).normalized();

    Matrix4 proj(
        v.x, v.y, v.z, 0,
        u.x, u.y, u.z, 0,
        d.x, d.y, d.z, 0,
        0, 0, 0, 1
    );

    Matrix4 persp(
        1.0/a, 0, 0, 0,
        0, ar/a, 0, 0,
        0, 0, -(-n-f)/(n-f), -2.0*f*n/(n-f),
        0, 0, -1, 0
    );

    matrixCached = true;
    return viewMatrix = persp*proj*trans;
}

Matrix4 OrthogonalCamera::getMatrix()
{
    return Matrix4(1, 0, 0, 0, 0, ar, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
}

Ray OrthogonalCamera::getViewRay(real x, real y) const
{
    return Ray(up*y/ar + (dir%up)*x, dir);
}

Ray PerspectiveCamera::getViewRay(real x, real y) const
{
    auto r = std::tan(deg(fov/2));
    auto d =  dir + up*y*r/ar + (dir%up).normalized()*x*r;
    auto p = pos;
    return Ray(p, d);
}

void Camera::setPos(const Vector3& pos)
{
    this->pos = pos;
    matrixCached = false;
}

void Camera::setDir(const Vector3& dir)
{
    this->dir = dir;
    matrixCached = false;
}

const Vector3& Camera::getPos() const
{
    return pos;
}

const Vector3& Camera::getDir() const
{
    return dir;
}

const Vector3& Camera::getUp() const
{
    return up;
}

const real Camera::getAspectRatio() const
{
    return ar;
}

const real PerspectiveCamera::getFov() const
{
    return fov;
}