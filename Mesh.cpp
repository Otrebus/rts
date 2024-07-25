#include "Math.h"
#include "Mesh.h"

Mesh::Mesh() : pos({ 0, 0, 0 }), dir({ 1, 0, 0 }), up({ 0, 0, 1 }), size({ 1, 1, 1 }), matrixCached(false)
{
}

Mesh::~Mesh()
{
}

void Mesh::setDirection(Vector3 dir, Vector3 up)
{
    this->dir = dir;
    this->up = up;
    matrixCached = false;
}

void Mesh::setPosition(Vector3 pos)
{
    this->pos = pos;
    matrixCached = false;
}

void Mesh::setSize(Vector3 size)
{
    this->size = size;
    matrixCached = false;
}

Material* Mesh::getMaterial() const
{
    return material;
}

void Mesh::setScene(Scene* s)
{
    scene = s;
}

Vector3 Mesh::getUp() const
{
    return up;
}

Vector3 Mesh::getDir() const
{
    return dir;
}

Vector3 Mesh::getPos() const
{
    return pos;
}

Matrix4 Mesh::getTransformationMatrix()
{
    if(matrixCached)
        return transformationMatrix;

    auto transM = getTranslationMatrix(getPos());
    auto dirM = getDirectionMatrix(getDir(), getUp());
    auto sizeM = getScalingMatrix(size);

    matrixCached = true;
    return transformationMatrix = sizeM*transM*dirM;
}