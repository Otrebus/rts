#pragma once
#pragma once
#define NOMINMAX

#include "Vector3.h"
#include <iostream>
#include "Matrix4.h"

class Terrain;
class Input;
class Ray;

class PointLight
{
public:
    PointLight();

    void setPos(const Vector3& pos);
    const Vector3& getPos() const;

    const Vector3& getColor() const;
    void setColor(const Vector3&);
    

protected:
    Vector3 pos;
    Vector3 color;
};
