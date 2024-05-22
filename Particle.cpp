#include "Particle.h"
#include "Input.h"
#include "Ray.h"


Particle::Particle(real start, const Vector3& pos, const Vector3& color) : start(start), pos(pos), color(color)
{
}


void Particle::setPos(const Vector3& pos)
{
    this->pos = pos;
}


const Vector3& Particle::getPos() const
{
    return pos;
}


Vector3 Particle::getVelocity() const
{
    return velocity;
}


void Particle::setVelocity(const Vector3& velocity)
{
    this->velocity = velocity;
}


const Vector3& Particle::getColor() const
{
    return color;
}

void Particle::setColor(const Vector3& color)
{
    this->color = color;
}

real Particle::getStart()
{
    return start;
}
