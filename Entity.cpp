
#define NOMINMAX
#include "Entity.h"
#include "Mesh3d.h"
#include "Vertex3d.h"
#include "Model3d.h"
#include "LineMesh3d.h"
#include "Line.h"
#include "Ray.h"
#include "Terrain.h"
#include "LambertianMaterial.h"
#include "LineMaterial.h"
#include <vector>
#include <array>
#include "BoundingBoxModel.h"


Entity::Entity(Vector3 pos, Vector3 dir, Vector3 up, real height, real width, real depth) : pos(pos), dir(dir), up(up)
{
}


Entity::~Entity()
{
}


void Entity::drawBoundingBox()
{
    boundingBoxModel->draw();
}

bool Entity::intersectBoundingBox(const Ray& ray)
{
    auto a = dir, b = up, c = dir%up, d = ray.dir, e = ray.pos-pos;

    auto det = a*(b%c);
    auto u2 = d*(b%c)/det;
    auto v2 = a*(d%c)/det;
    auto w2 = a*(b%d)/det;

    auto det2 = a*(b%c);
    auto u = e*(b%c)/det2;
    auto v = a*(e%c)/det2;
    auto w = a*(b%e)/det2;

    auto ray2 = Ray(Vector3(u, v, w), Vector3(u2, v2, w2));

    real tnear, tfar;
    if(boundingBox.intersect(ray2, tnear, tfar)) {
        return true;
    }
    return false;
}


void Entity::setSelected(bool selected)
{
    this->selected = selected;
    auto meshes = boundingBoxModel->getMeshes();
    auto& kd = ((LambertianMaterial*) meshes[0]->getMaterial())->Kd;
    kd = selected ? Vector3(0.8, 0, 0) : Vector3(0, 0.8, 0);
    auto& kd2 = ((LineMaterial*) meshes[1]->getMaterial())->Kd;
    kd2 = selected ? Vector3(0.8, 0, 0) : Vector3(0, 0.8, 0);
}


void Entity::setUp(Scene* scene)
{
    boundingBoxModel->setUp(scene);
}


void Entity::updateUniforms()
{
    boundingBoxModel->updateUniforms();
}


void Entity::plant(const Terrain& terrain)
{
    auto x = Vector3(dir.x, dir.y, 0).normalized();
    auto y = Vector3(-dir.y, dir.x, 0).normalized();
    auto z = Vector3(0, 0, 1);

    auto height = (boundingBox.c2.z-boundingBox.c1.z)/2;
    auto width = (boundingBox.c2.x-boundingBox.c1.x)/2;
    auto depth = (boundingBox.c2.y-boundingBox.c1.y)/2;

    auto a = pos + x*width + y*depth;
    auto b = pos - x*width + y*depth;
    auto c = pos - x*width - y*depth;
    auto d = pos + x*width - y*depth;

    auto ah = terrain.getElevation(a.x, a.y);
    auto bh = terrain.getElevation(b.x, b.y);
    auto ch = terrain.getElevation(c.x, c.y);
    auto dh = terrain.getElevation(d.x, d.y);

    Vector3 A = Vector3(a.x, a.y, ah);
    Vector3 B = Vector3(b.x, b.y, bh);
    Vector3 C = Vector3(c.x, c.y, ch);
    Vector3 D = Vector3(d.x, d.y, dh);

    up = ((C-B)%(A-B)).normalized();
    pos = Vector3(pos.x, pos.y, ((A+C)/2).z);
    auto l = Vector3(0, 0, 1)%x;
    dir = l%up;
    setPosition(pos);
    setDirection(dir.normalized(), up.normalized());
}


void Entity::setPosition(Vector3 pos)
{
    this->pos = pos;
    boundingBoxModel->setPosition(pos);
}

void Entity::setDirection(Vector3 dir, Vector3 up)
{
    boundingBoxModel->setDirection(dir, up);
    this->dir = dir;
    this->up = up;
}

Vector3 Entity::getPosition() const
{
    return pos;
}

void Entity::setTarget(Vector3 target)
{
    this->target = target;
}

Vector3 Entity::getTarget() const
{
    return target;
}

void Entity::setPath(std::vector<Vector2> path)
{
    this->path = path;
}

const std::vector<Vector2>& Entity::getPath() const
{
    return path;
}
