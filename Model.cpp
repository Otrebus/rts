#include <vector>
#include "Utils.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Model.h"
#include "LambertianMaterial.h"
#include "Camera.h"
#include "ObjReader.h"


Model3d::Model3d(std::string filename)
{
    ReadFromFile(filename);
}


Model3d::Model3d(const Mesh3d& mesh)
{
    AddMesh(mesh);
}


Model3d::Model3d()
{
}

Model3d::~Model3d()
{
    for(auto mat : materials)
        delete mat;
}

void Model3d::AddMesh(const Mesh3d& mesh)
{
    std::cout << "Pushing back";
    meshes.push_back(mesh);
}


void Model3d::Setup(Scene* scene)
{
    for(auto& mesh : meshes)
        mesh.Setup(scene);
}


void Model3d::TearDown(Scene* scene)
{
    for(auto& mesh : meshes)
        mesh.TearDown(scene);
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