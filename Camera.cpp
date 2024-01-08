#include "Camera.h"
#include "Input.h"

Matrix4 Camera::getMatrix(float fov, float ar)
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

    return persp*proj*trans;
}
