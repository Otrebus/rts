#pragma once

#include "Matrix4.h"
#include "Vector3.h"

class Terrain;
class Input;
class Ray;

class PointLight
{
public:
    PointLight(real start);

    void setPos(const Vector3& pos);
    const Vector3& getPos() const;

    Vector3 getVelocity() const;
    void setVelocity(const Vector3& velocity);

    const Vector3& getColor() const;
    void setColor(const Vector3&);

    real getStart();

    void update();

protected:
    Vector3 velocity;
    Vector3 pos;
    Vector3 color;
    real start;
};
