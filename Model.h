#include "Vector3.h"
#include "Vector2.h"
#include "ShaderProgram.h"


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
    std::vector<int> triangles;
    ShaderProgram* program;

    void BuildShader();
};


struct Model3d
{
    std::vector<Mesh3d> meshes;
};
