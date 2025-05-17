#include "Parser.h"
#include "Math.h"
#include "ModelLoader.h"


bool ModelManager::hasModel(const std::string& identifier)
{
    return templateMap.contains(identifier);
}

Model3d* ModelManager::instantiateModel(const std::string& identifier)
{
    assert(ModelManager::hasModel(identifier));
    auto baseModel = templateMap[identifier];
    auto newModel = new Model3d(*baseModel);
    return newModel;
}

void ModelManager::deleteModel(const std::string& identifier, Model3d* model)
{
    delete model; // TODO: superfluous function
}

Model3d* ModelManager::addModel(const std::string& name, Model3d* model)
{
    assert(!ModelManager::hasModel(name));
    templateMap[name] = model;
    return model;
}

Model3d* ModelManager::getModel(const std::string& name)
{
    return templateMap[name];
}

Model3d::~Model3d()
{
    for(auto& mesh : meshes)
        delete mesh;
}

Model3d::Model3d(Mesh3d& mesh)
{
    addMesh(mesh);
}

Model3d& Model3d::operator= (const Model3d& model)
{
    meshes.clear();
    for(auto mesh : model.meshes)
        addMesh(*new Mesh3d(*mesh));
    return *this;
}

Model3d::Model3d() : cloned(false)
{
}

Model3d::Model3d(const Model3d& model) : cloned(true)
{
    pos = model.pos;
    dir = model.dir;
    up = model.up;
    size = model.size;
    for(auto mesh : model.meshes)
        addMesh(*new Mesh3d(*mesh));
}

void Model3d::addMesh(Mesh3d& mesh)
{
    meshes.push_back(&mesh);
}


void Model3d::setScene(Scene* scene)
{
    for(auto& mesh : meshes)
    {
        mesh->setScene(scene);
    }
}


void Model3d::init()
{
    for(auto& mesh : meshes)
        mesh->init();
}


void Model3d::tearDown(Scene* scene)
{
    for(auto& mesh : meshes)
        mesh->tearDown();
}

Vector3 Model3d::getPosition()
{
    return pos;
}

Vector3 Model3d::getDirection()
{
    return dir;
}

Vector3 Model3d::getUp()
{
    return up;
}

void Model3d::draw(Material* mat)
{
    for(auto& mesh : meshes)
    {
        mesh->updateUniforms(mat);
        mesh->draw(mat);
    }
}

void Model3d::updateUniforms(Material* mat)
{
    for(auto& mesh : meshes)
        mesh->updateUniforms(mat);
}

void Model3d::setPosition(Vector3 pos)
{
    this->pos = pos;
    for(auto& mesh : meshes)
        mesh->setPosition(pos);
}

void Model3d::setSize(Vector3 size)
{
    this->size = size;
    for(auto& mesh : meshes)
        mesh->setSize(size);
}

void Model3d::setDirection(Vector3 dir, Vector3 up)
{
    assert(std::abs(dir.length()-1) < 0.01f);
    assert(std::abs(up.length()-1) < 0.01f);

    this->dir = dir;
    this->up = up;
    for(auto& mesh : meshes)
        mesh->setDirection(dir, up);
}

void Model3d::readFromFile(const std::string& file)
{
    ::readFromFile(this, file);
}

const std::vector<Mesh3d*>& Model3d::getMeshes() const
{
    return meshes;
}

Matrix4 Model3d::getTransformationMatrix()
{
    auto transM = getTranslationMatrix(pos);
    auto dirM = getDirectionMatrix(dir, up);
    auto sizeM = getScalingMatrix(size);
    return transM*dirM*sizeM;
}

void Model3d::transform(Matrix4 matrix)
{
    for(auto mesh : meshes)
        mesh->transform(matrix);
}

std::unordered_map<std::string, Model3d*> ModelManager::templateMap = std::unordered_map<std::string, Model3d*>();
