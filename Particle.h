#pragma once

#define NOMINMAX

#include "Vector3.h"
#include <iostream>
#include "Matrix4.h"

class Terrain;
class Input;
class Ray;

class Particle
{
public:
    Particle(real start);

    void setPos(const Vector3& pos);
    const Vector3& getPos() const;

    Vector3 getVelocity() const;
    void setVelocity(const Vector3& velocity);

    const Vector3& getColor() const;
    void setColor(const Vector3&);

    real getStart();
    
    void update();

// protected:
    Vector3 velocity;
    Vector3 pos;
    Vector3 color;
    real start;
};
