#define NOMINMAX
#include "Entity.h"
#include "Mesh3d.h"
#include "Vertex3d.h"
#include "Model3d.h"
#include "LineMesh3d.h"
#include "Line.h"
#include "Ray.h"
#include "Terrain.h"
#include "LambertianMaterial.h"
#include "LineMaterial.h"
#include <vector>
#include <array>
#include "BoundingBoxModel.h"
#include "SelectionMarkerMesh.h"
#include "Projectile.h"


Projectile::Projectile(Vector3 pos, Vector3 dir, Vector3 up) : Entity(pos, dir, up)
{
    depth = height = 0.02;
    width = 0.10;
	real w = width, d = depth, h = height;
    std::vector<Vector3> c = {
        { -w/2, -d/2, -h/2 }, // bottom-front-left
        {  w/2, -d/2, -h/2 }, // bottom-front-right
        {  w/2, -d/2,  h/2 }, // top-front-right
        { -w/2, -d/2,  h/2 }, // top-front-left
        { -w/2,  d/2, -h/2 }, // bottom-back-left
        {  w/2,  d/2, -h/2 }, // bottom-back-right
        {  w/2,  d/2,  h/2 }, // top-back-right
        { -w/2,  d/2,  h/2 }  // top-back-left
    };

    std::vector<std::vector<int>> cornerIndices = {
        { 0, 1, 2, 3 },
        { 1, 5, 6, 2 },
        { 4, 7, 6, 5 },
        { 0, 3, 7, 4 },
        { 2, 6, 7, 3 },
        { 0, 4, 5, 1 }
    };

    std::vector<int> triangles;
    std::vector<Vertex3d> vertices;

    for(auto ci : cornerIndices)
    {
        int t1[3] = { 0, 1, 2 }, t2[3] = { 0, 2, 3 };

        int j = vertices.size();
        for(int i = 0; i < 4; i++)
            vertices.push_back({ c[ci[i]], calcNormal(c[ci[0]], c[ci[1]], c[ci[2]]), { 0, 0 } });
        triangles.insert(triangles.end(), { j, j+1, j+2 } );
        triangles.insert(triangles.end(), { j, j+2, j+3 } );
    }

    auto material = new LambertianMaterial({ 0, 0.0, 0.0 });

    auto projectileMesh = new Mesh3d(vertices, triangles, material);
    projectileModel = new Model3d(*projectileMesh);
    
    boundingBoxModel = new BoundingBoxModel(pos, dir, up, width, depth, height);

    setPosition(pos);
    setDirection(dir, up);
}

void Projectile::init(Scene* scene)
{
    this->scene = scene;
    projectileModel->init(scene);
}

void Projectile::setPosition(Vector3 pos)
{
    this->pos = pos;
    boundingBoxModel->setPosition(pos);
    projectileModel->setPosition(pos);
}

void Projectile::setDirection(Vector3 dir, Vector3 up)
{
    this->dir = dir;
    boundingBoxModel->setDirection(dir, up);
    projectileModel->setDirection(dir, up);
}

void Projectile::update(real dt)
{
    velocity += Vector3( { 0, 0, -1 } )*dt;
    setDirection(velocity.normalized(), ((velocity%up%velocity).normalized()));
    setPosition(pos + velocity*dt);
}

void Projectile::draw()
{
    projectileModel->updateUniforms();
    projectileModel->draw();
}