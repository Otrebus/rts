#define NOMINMAX

#include "Entity.h"
#include "Ray.h"
#include "Terrain.h"
#include "Vector3.h"


Entity::Entity(Vector3 pos, Vector3 dir, Vector3 up) : pos(pos), dir(dir), up(up), velocity({ 0, 0, 0 }), lastBumped(0)
{
}


Entity::~Entity()
{
}


void Entity::drawBoundingBox()
{
}


bool Entity::intersectBoundingBox(const Ray& ray)
{
    auto a = dir, b = dir%up, c = up;
    auto ray2 = Ray(rebaseOrtho(ray.pos-pos, a, b, c), rebaseOrtho(ray.dir, a, b, c));

    real tnear, tfar;
    if(boundingBox.intersect(ray2, tnear, tfar))
        return true;
    return false;
}


bool Entity::intersectBoundingBox(Vector3 p1, Vector3 p2)
{
    auto a = dir, b = dir%up, c = up;
    Ray ray = Ray(p1, (p2-p1).normalized());
    auto ray2 = Ray(rebaseOrtho(ray.pos-pos, a, b, c), rebaseOrtho(ray.dir, a, b, c));

    real tnear, tfar;
    if(boundingBox.intersect(ray2, tnear, tfar))
    {
        return tnear < (p2-p1).length();
    }
    return false;
}

std::pair<Vector3, Vector3> Entity::getBoundingBoxIntersection(Vector3 p1, Vector3 p2)
{
    auto a = dir, b = dir%up, c = up;
    Ray ray = Ray(p1, (p2-p1).normalized());
    auto ray2 = Ray(rebaseOrtho(ray.pos-pos, a, b, c), rebaseOrtho(ray.dir, a, b, c));

    real tnear, tfar;
    auto normal = boundingBox.getNormal(ray2, tnear, tfar);
    return { p1 + tnear*(p2-p1), normal.x * a + normal.y * b + normal.z * c };
}

void Entity::init(Scene* scene)
{
    this->scene = scene;
}


void Entity::updateUniforms()
{
}


void Entity::plant(const Terrain& terrain)
{
    auto x = Vector3(geoDir.x, geoDir.y, 0).normalized();
    auto y = Vector3(-geoDir.y, geoDir.x, 0).normalized();
    auto z = Vector3(0, 0, 1).normalized();

    auto height = (boundingBox.c2.z-boundingBox.c1.z)/2;
    auto length = (boundingBox.c2.x-boundingBox.c1.x)/2;
    auto width = (boundingBox.c2.y-boundingBox.c1.y)/2;

    auto a = geoPos.to3() + x*length + y*width;
    auto b = geoPos.to3() - x*length + y*width;
    auto c = geoPos.to3() - x*length - y*width;
    auto d = geoPos.to3() + x*length - y*width;

    auto ah = terrain.getElevation(a.x, a.y);
    auto bh = terrain.getElevation(b.x, b.y);
    auto ch = terrain.getElevation(c.x, c.y);
    auto dh = terrain.getElevation(d.x, d.y);

    Vector3 A = Vector3(a.x, a.y, ah);
    Vector3 B = Vector3(b.x, b.y, bh);
    Vector3 C = Vector3(c.x, c.y, ch);
    Vector3 D = Vector3(d.x, d.y, dh);

    // TODO: the up vector shouldn't just be C, B and A vectors since the height of all 4 are averaged
    up = ((C-B)%(A-B)).normalized();
    pos = Vector3(geoPos.x, geoPos.y, ((A+B+C+D)/4.f).z) + up*(height);
    dir = y%up;
    setPosition(pos);
    setDirection(dir.normalized(), up.normalized());
}


void Entity::setPosition(Vector3 pos)
{
    this->pos = pos;
}

void Entity::setDirection(Vector3 dir, Vector3 up)
{
    this->dir = dir;
    this->up = up;
}

void Entity::setDead()
{
    dead = true;
}

Vector3 Entity::getPosition() const
{
    return pos;
}

Vector3 Entity::getVelocity() const
{
    return velocity;
}

void Entity::setVelocity(Vector3 velocity)
{
    this->velocity = velocity;
}

void Entity::setGeoPosition(Vector2 pos)
{
    geoPos = pos;
}

Vector2 Entity::getGeoPosition() const
{
    return geoPos;
}

Vector2 Entity::getGeoVelocity() const
{
    return geoVelocity;
}

void Entity::setId(int id)
{
    this->id = id;
}

int Entity::getId() const
{
    return id;
}