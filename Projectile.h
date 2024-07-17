#pragma once

#include "Entity.h"

class Projectile : public Entity
{
public:
    Projectile(Vector3 pos, Vector3 dir, Vector3 up, Entity* owner);
    ~Projectile();
    void draw(Material* mat = nullptr);

    void setPosition(Vector3 pos);
    void setDirection(Vector3 pos, Vector3 up);

    void update(real dt);
    void init(Scene* scene);

protected:
    Model3d* projectileModel;
    Entity* owner;
};
