#include "BoundingBox.h"
#include "Ray.h"
#include "Utils.h"
#include "Vector3.h"

BoundingBox::BoundingBox(const Vector3& c1, const Vector3& c2) : c1(c1), c2(c2)
{
}

BoundingBox::~BoundingBox()
{
}

BoundingBox::BoundingBox()
{
    c1 = { inf, inf, inf };
    c2 = { -inf, -inf, -inf };
}

bool BoundingBox::intersect(const Ray& ray, real& tnear, real& tfar) const
{
    real t1, t2;
    tfar = inf;
    tnear = -inf;

    for(int u = 0; u < 3; u++)
    {
        if(ray.dir[u] == 0)
        {
            if(ray.pos[u] > c2[u] || ray.pos[u] < c1[u])
                return false;
        }
        else
        {
            t1 = (c1[u] - ray.pos[u]) / ray.dir[u];
            t2 = (c2[u] - ray.pos[u]) / ray.dir[u];
            if(t1 > t2)
                std::swap(t1, t2);
            if(t1 > tnear)
                tnear = t1;
            if(t2 < tfar)
                tfar = t2;
            if(tnear > tfar)
                return false;
        }
    }
    return tnear > 0;
}

Vector3 BoundingBox::getNormal(const Ray& ray, real& tnear, real& tfar) const
{
    real t1, t2;
    tfar = inf;
    tnear = -inf;

    Vector3 N[3] = { { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 } }, normal;
    for(int u = 0; u < 3; u++)
    {
        t1 = (c1[u] - ray.pos[u]) / ray.dir[u];
        t2 = (c2[u] - ray.pos[u]) / ray.dir[u];
        if(t1 > t2)
            std::swap(t1, t2);
        if(t1 > tnear)
        {
            tnear = t1;
            normal = ray.dir[u] > 0 ? -N[u] : N[u];
        }
        if(t2 < tfar)
            tfar = t2;
    }
    return normal;
}