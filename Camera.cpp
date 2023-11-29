#include "Camera.h"

Matrix4 Camera::GetMatrix(float fov, float ar)
{
    float pi = 3.141592653589793;
    float a = std::tan(pi*fov/180/2);
    float n = -0.001, f = -1000.0;

    Matrix4 trans(
        1, 0, 0, -pos.x,
        0, 1, 0, -pos.y,
        0, 0, 1, -pos.z,
        0, 0, 0, 1
    );

    Vector3 d = -dir.Normalized();
    Vector3 u1 = this->up;
    Vector3 v = (u1^d).Normalized();
    Vector3 u = (d^v).Normalized();

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

    return persp*proj*trans;
}


CameraControl::CameraControl(int panningX, int panningY, bool panning, real theta, real phi, Camera* cam) :
    panningX(panningX), panningY(panningY), panning(panning), theta(theta), phi(phi), cam(cam)
{
    setAngle(theta, phi);
}


real CameraControl::getPhi()
{
    return phi;
}


real CameraControl::getTheta()
{
    return theta;
}


Camera* CameraControl::getCamera()
{ 
    return cam;
}


void CameraControl::setAngle(real theta, real phi) {
    Vector3 f(0, 1, 0), r(1, 0, 0);

    this->theta = theta;
    this->phi = phi;

    cam->up = Vector3(0, 0, 1);
    Vector3 h = f*std::sin(theta) + r*std::cos(theta);
    cam->dir = cam->up*std::sin(phi) + h*std::cos(phi);
}


void CameraControl::moveForward(real t) {
    cam->pos = cam->pos + cam->dir*t*3;
}


void CameraControl::moveRight(real t) {
    cam->pos = cam->pos - (cam->up^cam->dir)*t*3;
}
