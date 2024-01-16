#include <tuple>
#include "Vector3.h"
#include "Ray.h"


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