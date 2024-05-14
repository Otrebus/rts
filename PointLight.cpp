#include "PointLight.h"
#include "Input.h"
#include "Ray.h"


PointLight::PointLight()
{
}


void PointLight::setPos(const Vector3& pos)
{
    this->pos = pos;
}


const Vector3& PointLight::getPos() const
{
    return pos;
}


const Vector3& PointLight::getColor() const
{
    return color;
}

void PointLight::setColor(const Vector3& color)
{
    this->color = color;
}
