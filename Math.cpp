#include <tuple>
#include "Vector3.h"
#include "Ray.h"
#include "Matrix4.h"
#include "Camera.h"
#include "Math.h"


std::tuple<double, double, double> intersectTriangle(const Vector3& v0, const Vector3& v1, const Vector3& v2, const Ray& ray)
{
    double u, v, t;
    const Vector3& D = ray.dir;

    Vector3 E1 = v1-v0, E2 = v2-v0;
    Vector3 T = ray.pos - v0;

    Vector3 P = E2%T, Q = E1%D;

    double det = E2*Q;
    if(!det)
        return { -inf, -inf, -inf };
    u = ray.dir*P/det;

    if(u > 1 || u < 0)
        return { -inf, -inf, -inf };

    v = T*Q/det;

    if(u+v > 1 || u < 0 || v < 0)
        return { -inf, -inf, -inf };

    t = E1*P/det;

    return { t <= 0 ? -inf : t, u, v };
}

Matrix4 getTranslationMatrix(Vector3 v)
{
    return Matrix4(
        1, 0, 0, v.x,
        0, 1, 0, v.y,
        0, 0, 1, v.z,
        0, 0, 0, 1
    );
}

Matrix4 getDirectionMatrix(Vector3 dir, Vector3 up)
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


const Matrix4 identityMatrix(
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
);
const real pi = std::acos(-1);


const Vector3& getViewRay(Camera& cam, real y, real x)
{
    auto d = std::tan(pi*cam.fov/180/2);
    return cam.dir + cam.up*y*d/cam.ar + (cam.dir%cam.up).normalized()*x*d;
}

real resToScreenX(real x, int xres)
{
    return real(2*x)/xres - 1;
}

real resToScreenY(real y, int yres)
{
    return -(real(2*y)/yres - 1);
}