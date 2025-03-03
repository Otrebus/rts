#include "Camera.h"
#include "Math.h"
#include "Vector2.h"
#include "Vector3.h"


Matrix4 getTranslationMatrix(const Vector3& v)
{
    return Matrix4(
        1, 0, 0, v.x,
        0, 1, 0, v.y,
        0, 0, 1, v.z,
        0, 0, 0, 1
    );
}

Matrix4 getDirectionMatrix(const Vector3& dir, const Vector3& up)
{
    auto u = (up%dir).normalized();
    return Matrix4(
        dir.x, u.x, up.x, 0,
        dir.y, u.y, up.y, 0,
        dir.z, u.z, up.z, 0,
        0, 0, 0, 1
    );
}

Matrix4 getNormalMatrix(const Matrix4& m)
{
    auto& a11 = m.m_val[0][0], &a12 = m.m_val[0][1], &a13 = m.m_val[0][2],
        &a21 = m.m_val[1][0], &a22 = m.m_val[1][1], &a23 = m.m_val[1][2],
        &a31 = m.m_val[2][0], &a32 = m.m_val[2][1], &a33 = m.m_val[2][2];

    auto det = a11*a22*a33 + a12*a23*a31 + a13*a21*a32 - a11*a23*a32 - a31*a22*a13 - a33*a21*a12;

    auto b11 = a22*a33-a23*a32, b12 = -(a21*a33-a23*a31), b13 = a21*a32-a22*a31,
        b21 = -(a12*a33-a13*a32), b22 = a11*a33-a13*a31, b23 = -(a11*a32-a12*a31),
        b31 = a12*a23-a13*a22, b32 = -(a11*a23-a13*a21), b33 = a11*a22-a12*a21;

    return Matrix4(
        b11, b12, b13, 0,
        b21, b22, b23, 0,
        b31, b32, b33, 0,
        0, 0, 0, 1
    )/det;
}

Matrix4 getScalingMatrix(const Vector3& scaling)
{
    return Matrix4(
        scaling.x, 0, 0, 0,
        0, scaling.y, 0, 0,
        0, 0, scaling.z, 0,
        0, 0, 0, 1
    );
}

const Matrix4 identityMatrix(
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
);
const real pi = std::acos(-1.0f);
const real eps = 1e-6f;
const real gravity = 1; // TODO: put in another file eventually, not a math constant

Vector2 resToScreen(real x, real y, int xres, int yres)
{
    return { real(2*x)/xres - 1, -(real(2*y)/yres - 1) };
}

Vector3 calcNormal(Vector3 a, Vector3 b, Vector3 c)
{
    return ((c-b)%(a-b)).normalized();
}

real deg(real rad)
{
    return pi*rad/180;
}
