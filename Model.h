#include "Vector3.h"
#include "Vector2.h"
#include "Matrix4.h"
#include "ShaderProgram.h"
#include "Material.h"


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
    Material* material;

    GLuint VBO, VAO, EBO;
    ShaderProgram* program;

    void Setup();
    void Draw();

    void SetTransformationMatrix(const Matrix4&);
    void SetCameraPosition(const Vector3&);
};


struct Model3d
{
public:
    std::vector<Mesh3d> meshes;
    void Setup();
    void Draw();

    void SetTransformationMatrix(const Matrix4&);
    void SetCameraPosition(const Vector3&);
};
