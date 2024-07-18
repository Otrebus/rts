#include "GeometryUtils.h"
#include "Ray.h"
#include "Unit.h"
#include <cassert>

real intersectRayCircle(Vector2 pos, Vector2 dir, Vector2 c, real radius)
{
    auto p0 = pos-c, p1 = dir;

    auto C = (p0.length2() - radius*radius)/p1.length2();
    auto B = 2*(p0.x*p1.x + p0.y*p1.y)/p1.length2();

    auto s = std::sqrt(B*B/4 - C);
    if(B*B/4 - C < 0)
        return -inf;
    auto t1 = -B/2 - s, t2 = -B/2 + s;

    return t1 > 0 ? t1 : t2 > 0 ? t2 : -inf;
}

real intersectRaySegment(Vector2 pos, Vector2 dir, Vector2 p1, Vector2 p2)
{
    auto e = p2 - p1;
    auto c = pos - p1;
    auto det = (-dir.x*e.y + e.x*dir.y);

    auto s = (c.x*e.y - e.x*c.y)/det;
    auto t = (-dir.x*c.y + c.x*dir.y)/det;
    if(t >= 0 && t <= 1 && s >= 0)
        return s;
    return -inf;
}


std::tuple<real, Vector2, real> distPointLine(Vector2 p, Vector2 p1, Vector2 p2)
{
    auto e = (p2-p1).normalized();
    auto s = (p-p1)*e;
    return { (p - (p1 + e*s)).length(), p1 + e*s, s };
}


std::pair<real, Vector2> intersectCircleTrianglePath(Vector2 pos, real radius, Vector2 dir, Vector2 p1, Vector2 p2, Vector2 p3)
{
    auto pp1 = (p1-p2).perp().normalized(), pp2 = (p2-p3).perp().normalized(), pp3 = (p3-p1).perp().normalized();
    auto e11 = p1 + pp1*radius, e12 = p2 + pp1*radius;
    auto e21 = p2 + pp2*radius, e22 = p3 + pp2*radius;
    auto e31 = p3 + pp3*radius, e32 = p1 + pp3*radius;

    // First check if we're closer than the radius of the circle to the triangle
    if(auto [t, q, s] = distPointLine(pos, p1, p2); t < radius && s > 0 && s < (p2-p1).length())
        return { 0, (p1-q).perp().normalized() };

    if(auto [t, q, s] = distPointLine(pos, p2, p3); t < radius && s > 0 && s < (p3-p2).length())
        return { 0, (p2-q).perp().normalized() };

    if(auto [t, q, s] = distPointLine(pos, p3, p1); t < radius && s > 0 && s < (p1-p3).length())
        return { 0, (p3-q).perp().normalized() };

    if(auto t = (pos - p1).length(); t < radius)
        return { 0, (pos - p1).normalized() };

    if(auto t = (pos - p2).length(); t < radius)
        return { 0, (pos - p2).normalized() };

    if(auto t = (pos - p3).length(); t < radius)
        return { 0, (pos - p3).normalized() };

    real minT = inf;
    Vector2 norm;

    real t = intersectRaySegment(pos, dir, e11, e12);
    if(t > 0)
        minT = t, norm = pp1;

    t = intersectRaySegment(pos, dir, e21, e22);
    if(t > 0 && t < minT)
        minT = t, norm = pp2;

    t = intersectRaySegment(pos, dir, e31, e32);
    if(t > 0 && t < minT)
        minT = t, norm = pp3;

    t = intersectRayCircle(pos, dir, p1, radius);

    // TODO: we probably want different normals here
    if(t > 0 && t < minT)
    {
        minT = t;
        norm = (pos+dir*t-p1).normalized();
        /*if((pos-p1)*(pp1) > 0)
            norm = pp1;
        else
            norm = pp2;*/
    }

    t = intersectRayCircle(pos, dir, p2, radius);
    if(t > 0 && t < minT)
    {
        minT = t;
        norm = (pos+dir*t-p2).normalized();
        /*if((pos-p2)*(pp2) > 0)
            norm = pp2;
        else
            norm = pp1;*/
    }

    t = intersectRayCircle(pos, dir, p3, radius);
    if(t > 0 && t < minT)
    {
        minT = t;
        norm = (pos+dir*t-p3).normalized();
        /*if((pos-p3)*(pp3) > 0)
            norm = pp3;
        else
            norm = pp2;*/
    }

    return { minT, norm };
}

Vector3 rebaseOrtho(const Vector3& v, const Vector3& a, const Vector3& b, const Vector3& c)
{
    assert(std::abs(a.length()-1) < 0.01);
    assert(std::abs(b.length()-1) < 0.01);
    assert(std::abs(c.length()-1) < 0.01);

    return { v*a, v*b, v*c };
}


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


real getArrivalRadius(Vector2 p, const std::vector<Unit*>& units)
{
    real L = 0.5, R = 10.0;
    while(R - L > 0.1)
    {
        auto M = (R+L)/2;
        real sum = 0;
        for(auto& unit : units)
        {
            if(auto w = (unit->geoPos-p).length(); w < M)
                sum += 1.5*1.5*pi;
        }
        if(sum < pi*M*M)
            R = M;
        else
            L = M;
    }
    return L;
}
