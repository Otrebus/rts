#include "Mesh.h"

Mesh::Mesh() : pos({ 0, 0, 0 }), dir({ 1, 0, 0 }), up({ 0, 0, 1 })
{
}

Mesh::~Mesh()
{
}

void Mesh::setDirection(Vector3 dir, Vector3 up)
{
    this->dir = dir;
    this->up = up;
}

void Mesh::setPosition(Vector3 pos)
{
    this->pos = pos;
}

Material* Mesh::getMaterial() const
{
    return material;
}
