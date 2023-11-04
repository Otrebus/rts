#include <vector>
#include "Vector2.h"
#include "Vector3.h"
#include "Model.h"
#include "LambertianMaterial.h"
#include "Camera.h"

void Model3d::Setup(Scene* scene)
{
    for(auto& mesh : meshes)
        mesh.Setup(scene);
}


void Model3d::Draw()
{
    for(auto& mesh : meshes)
        mesh.Draw();
}

void Model3d::UpdateUniforms()
{
    for(auto& mesh : meshes)
        mesh.UpdateUniforms();
}