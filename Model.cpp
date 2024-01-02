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
    readFromFile(filename);
}


Model3d::Model3d(const Mesh3d& mesh)
{
    addMesh(mesh);
}


Model3d::Model3d()
{
}

Model3d::~Model3d()
{
    for(auto mat : materials)
        delete mat;
}

void Model3d::addMesh(const Mesh3d& mesh)
{
    std::cout << "Pushing back";
    meshes.push_back(mesh);
}


void Model3d::setup(Scene* scene)
{
    for(auto& mesh : meshes)
        mesh.setup(scene);
}


void Model3d::tearDown(Scene* scene)
{
    for(auto& mesh : meshes)
        mesh.tearDown(scene);
}


void Model3d::draw()
{
    for(auto& mesh : meshes)
        mesh.draw();
}

void Model3d::updateUniforms()
{
    for(auto& mesh : meshes)
        mesh.updateUniforms();
}