#pragma once
#include "Vector2.h"
#include "Vector3.h"


struct Vertex3d
{
    Vertex3d(Vector3 pos, Vector3 normal, Vector2 tex);
    Vertex3d() {}
    Vertex3d(std::initializer_list<float> list);
    Vertex3d(real x, real y, real z, real nx, real ny, real nz, real tx, real ty);

    Vector3 pos;
    Vector3 normal;
    Vector2 tex;
};

struct MeshVertex3d
{
    MeshVertex3d(Vector3 pos, Vector3 normal, Vector2 tex);
    MeshVertex3d() {}
    MeshVertex3d(std::initializer_list<float> list);
    MeshVertex3d(real x, real y, real z, real nx, real ny, real nz, real tx, real ty);

    Vector3 pos;
    Vector3 normal;
    Vector2 tex;
    int selected;
};