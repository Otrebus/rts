#include "Mesh3d.h"
#include "Vector3.h"
#include "Vector2.h"
#include "Matrix4.h"
#include "ShaderProgram.h"
#include "Material.h"
#include "Camera.h"


struct Model3d
{
public:
    std::vector<Mesh3d> meshes;
    void Setup(Scene* scene);
    void Draw();

    void UpdateUniforms();
};
