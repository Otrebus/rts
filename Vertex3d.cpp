#pragma once
#include "Vertex3d.h"


Vertex3::Vertex3(Vector3 pos, Vector3 normal, Vector2 tex) : pos(pos), normal(normal), tex(tex) {}


Vertex3::Vertex3(std::initializer_list<float> list)
{
    auto it = list.begin();
    auto x = *it++, y = *it++, z = *it++;
    pos = Vector3(x, y, z);
    auto nx = *it++, ny = *it++, nz = *it++;
    normal = Vector3(nx, ny, nz);
    auto tx = *it++, ty = *it++;
    tex = Vector2(tx, ty);
}


Vertex3::Vertex3(real x, real y, real z, real nx, real ny, real nz, real tx, real ty)
{
    pos = Vector3(x, y, z);
    normal = Vector3(nx, ny, nz);
    tex = Vector2(tx, ty);
}

MeshVertex3::MeshVertex3(real x, real y, real z, real nx, real ny, real nz, real tx, real ty)
{
    pos = Vector3(x, y, z);
    normal = Vector3(nx, ny, nz);
    tex = Vector2(tx, ty);
}

MeshVertex3::MeshVertex3(std::initializer_list<float> list)
{
    auto it = list.begin();
    auto x = *it++, y = *it++, z = *it++;
    pos = Vector3(x, y, z);
    auto nx = *it++, ny = *it++, nz = *it++;
    normal = Vector3(nx, ny, nz);
    auto tx = *it++, ty = *it++;
    tex = Vector2(tx, ty);
}

bool Vertex3::operator==(const Vertex3& v) const
{
    return pos == v.pos && tex == v.tex && normal == v.normal;
}
