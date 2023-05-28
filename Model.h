#include "Vector3.h"
#include "Vector2.h"
#include "Shader.h"


class Vertex3d
{
    Vector3 pos;
    Vector3 normal;
    Vector2 tex;
};


class Mesh3d
{
    std::vector<Vertex3d> v;
    std::vector<int> tri;
    Shader* shader;
};


class Model3d
{
    std::vector<Mesh3d> v;
};
