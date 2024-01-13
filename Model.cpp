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


Model3d::Model3d(Mesh3d& mesh)
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

void Model3d::addMesh(Mesh3d& mesh)
{
    meshes.push_back(&mesh);
}


void Model3d::setup(Scene* scene)
{
    for(auto& mesh : meshes) {
        std::cout << "mesh is " << mesh << std::endl;
        mesh->setup(scene);
        std::cout << "mesh " << mesh << " has scene " << mesh->scene << std::endl;
    }
}


void Model3d::tearDown(Scene* scene)
{
    for(auto& mesh : meshes)
        mesh->tearDown(scene);
}


void Model3d::draw()
{
    for(auto& mesh : meshes)
        mesh->draw();
}

void Model3d::updateUniforms()
{
    for(auto& mesh : meshes)
        mesh->updateUniforms();
}