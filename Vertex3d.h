#pragma once
#include "Vector2.h"
#include "Vector3.h"


struct Vertex3
{
    Vertex3(Vector3 pos, Vector3 normal, Vector2 tex);
    Vertex3() {}
    Vertex3(std::initializer_list<float> list);
    Vertex3(real x, real y, real z, real nx, real ny, real nz, real tx, real ty);

    bool operator==(const Vertex3& v) const;

    Vector3 pos;
    Vector3 normal;
    Vector2 tex;
};

struct MeshVertex3
{
    MeshVertex3(Vector3 pos, Vector3 normal, Vector2 tex);
    MeshVertex3() {}
    MeshVertex3(std::initializer_list<float> list);
    MeshVertex3(real x, real y, real z, real nx, real ny, real nz, real tx, real ty);

    Vector3 pos;
    Vector3 normal;
    Vector2 tex;
    int selected;
};