#include "BoundingBox.h"
#include "Utils.h"
#include "Ray.h"

BoundingBox::BoundingBox(const Vector3& c1, const Vector3& c2) : c1(c1), c2(c2)
{
}

BoundingBox::~BoundingBox()
{
}

BoundingBox::BoundingBox()
{
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
    return true;
}
