#pragma once

#include <string>
#include <tuple>
#include <vector>
#include <map>
#include "Vector3.h"
#include "Vector2.h"

class Material;
class TriangleMesh;
class Model3d;

Model3d ReadFromFile(const std::string& file);
std::map<std::string, Material*> ReadMaterialFile(const std::string& matfilestr);


class ObjTriangle;


struct ObjVertex
{
    ObjVertex(Vector3 position, Vector3 normal, Vector2 texture) : position(position), normal(normal), texture(texture) {}

    Vector3 position;
    Vector3 normal;
    Vector2 texture;

    std::vector<ObjTriangle*> triangles;
};


struct ObjTriangle
{
    ObjTriangle(ObjVertex* v0, ObjVertex* v1, ObjVertex* v2) : v0(v0), v1(v1), v2(v2) {}
    ObjVertex *v0, *v1, *v2;

    Vector3 GetNormal() const
    {
        Vector3 normal = (v1->position-v0->position)%(v2->position-v0->position);
        normal.Normalize();
        return normal;
    }
};
