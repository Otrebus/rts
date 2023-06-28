#include <vector>
#include "Vector2.h"
#include "Vector3.h"
#include "Model.h"
#include "LambertianMaterial.h"


void Mesh3d::BuildShader()
{
    ShaderProgram s;

    material = new LambertianMaterial(Vector3(0.5, 0.5, 0.2));

    Shader vertexShader("vertex.glsl", GL_VERTEX_SHADER);

    s.AddShaders(vertexShader, material->GetShader());
    s.Use();
}