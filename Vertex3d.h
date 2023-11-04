#pragma once
#include "Vector3.h"
#include "Vector2.h"

struct Vertex3d
{
    Vertex3d(Vector3 pos, Vector3 normal, Vector2 tex) : pos(pos), normal(normal), tex(tex) {}
    Vertex3d(std::initializer_list<float> list) 
    {
        auto it = list.begin();
        auto x = *it++, y = *it++, z = *it++;
        pos = Vector3(x, y, z);
        auto nx = *it++, ny = *it++, nz = *it++;
        normal = Vector3(nx, ny, nz);
        auto tx = *it++, ty = *it++;
        tex = Vector2(tx, ty);
    }
    Vector3 pos;
    Vector3 normal;
    Vector2 tex;
};