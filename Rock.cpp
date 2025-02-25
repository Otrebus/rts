#include "Entity.h"
#include "LambertianMaterial.h"
#include "Model3d.h"
#include "Rock.h"
#include "FogOfWarMaterial.h"


Rock::Rock(Vector3 pos, Vector3 dir, Vector3 up, Terrain* terrain) : Entity(pos, dir, up)
{
    model = ModelManager::instantiateModel("Rock");

    model->setScene(scene);
    model->setPosition(pos);
    model->setDirection(dir, up);
}

void Rock::init(Scene* scene)
{
    this->scene = scene;
    model->setScene(scene);
}

void Rock::setPosition(Vector3 pos)
{
    this->pos = pos;
    model->setPosition(pos);
}

void Rock::setDirection(Vector3 dir, Vector3 up)
{
    this->dir = dir;
    this->up = up;
    model->setDirection(dir, up);
}

void Rock::loadModels()
{
    auto model = new Model3d();
    model->readFromFile("Rock.obj");

    real length = 1;

    BoundingBox bb1;

    for(auto& mesh : model->getMeshes())
    {
        for(auto& v : ((Mesh3d*)mesh)->v)
        {
            v.pos = Vector3(v.pos.x, v.pos.z, v.pos.y);
            v.normal = -Vector3(v.normal.x, v.normal.z, v.normal.y);
            bb1.c1.x = std::min(bb1.c1.x, v.pos.x);
            bb1.c1.y = std::min(bb1.c1.y, v.pos.y);
            bb1.c1.z = std::min(bb1.c1.z, v.pos.z);
            bb1.c2.x = std::max(bb1.c2.x, v.pos.x);
            bb1.c2.y = std::max(bb1.c2.y, v.pos.y);
            bb1.c2.z = std::max(bb1.c2.z, v.pos.z);
        }
    }

    auto l = (bb1.c2.x - bb1.c1.x);
    auto w = (bb1.c2.y - bb1.c1.y);
    auto h = (bb1.c2.z - bb1.c1.z);
    auto ratio = length/l;

    BoundingBox bb[3];
    int i = 0;

    for(auto& mesh : model->getMeshes())
    {
        for(auto& v : ((Mesh3d*)mesh)->v)
        {
            bb[i].c1.x = std::min(bb[i].c1.x, v.pos.x);
            bb[i].c1.y = std::min(bb[i].c1.y, v.pos.y);
            bb[i].c1.z = std::min(bb[i].c1.z, v.pos.z);
            bb[i].c2.x = std::max(bb[i].c2.x, v.pos.x);
            bb[i].c2.y = std::max(bb[i].c2.y, v.pos.y);
            bb[i].c2.z = std::max(bb[i].c2.z, v.pos.z);
        }
    }
    for(auto& mesh : model->getMeshes())
    {
        for(auto& v : ((Mesh3d*)mesh)->v)
        {
            v.pos -= Vector3((bb[i].c2.x + bb[i].c1.x)/2, (bb[i].c2.y + bb[i].c1.y)/2, (bb[i].c2.z + bb[i].c1.z)/2);
            v.pos *= length/l;
        }
    }
    model->init();

    auto height = h*ratio;
    auto width = w*ratio;

    vehicleBoundingBox = BoundingBox(Vector3(-length/2, -width/2, -height/2), Vector3(length/2, width/2, height/2));
}

void Rock::draw(Material* mat)
{
    model->draw();
}

void Rock::update(real dt)
{
    plant(*scene->getTerrain());
}

void Rock::updateUniforms()
{
    model->updateUniforms();
}
