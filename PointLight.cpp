#include "PointLight.h"


PointLight::PointLight(real start)
{
    this->start = start;
}


void PointLight::setPos(const Vector3& pos)
{
    this->pos = pos;
}


const Vector3& PointLight::getPos() const
{
    return pos;
}


Vector3 PointLight::getVelocity() const
{
    return velocity;
}


void PointLight::setVelocity(const Vector3& velocity)
{
    this->velocity = velocity;
}


const Vector3& PointLight::getColor() const
{
    return color;
}

void PointLight::setColor(const Vector3& color)
{
    this->color = color;
}

real PointLight::getStart()
{
    return start;
}
