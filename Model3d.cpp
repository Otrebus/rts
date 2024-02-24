#include <vector>
#include "Utils.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Model3d.h"
#include "LambertianMaterial.h"
#include "Camera.h"
#include "ObjReader.h"


Model3d::Model3d(std::string filename)
{
    readFromFile(filename);
}


Model3d::Model3d(Mesh& mesh)
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


void Model3d::addMesh(Mesh& mesh)
{
    meshes.push_back(&mesh);
}


void Model3d::setUp(Scene* scene)
{
    for(auto& mesh : meshes) {
        mesh->setUp(scene);
    }
}


void Model3d::tearDown(Scene* scene)
{
    for(auto& mesh : meshes)
        mesh->tearDown(scene);
}


void Model3d::draw()
{
    for(auto& mesh : meshes) {
        mesh->updateUniforms();
        mesh->draw();
    }
}


void Model3d::updateUniforms()
{
    for(auto& mesh : meshes)
        mesh->updateUniforms();
}


void Model3d::setPosition(Vector3 pos)
{
    for(auto& mesh : meshes)
        mesh->setPosition(pos);
}


void Model3d::setDirection(Vector3 dir, Vector3 up)
{
    for(auto& mesh : meshes)
        mesh->setDirection(dir, up);
}


const std::vector<Mesh*>& Model3d::getMeshes() const
{
    return meshes;
}
