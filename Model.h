#include "Vector3.h"
#include "Vector2.h"
#include "Shader.h"


struct Vertex3d
{
    Vertex3d(Vector3 pos, Vector3 normal, Vector2 tex) : pos(pos), normal(normal), tex(tex) {}
    Vector3 pos;
    Vector3 normal;
    Vector2 tex;
};


struct Mesh3d
{
    std::vector<Vertex3d> v;
    std::vector<int> tri;
    Shader* shader;
};


struct Model3d
{
    std::vector<Mesh3d> meshes;
};
