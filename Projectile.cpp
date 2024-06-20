#define NOMINMAX
#include "Entity.h"
#include "Line.h"
#include "LineMaterial.h"
#include "LineMesh3d.h"
#include "Mesh3d.h"
#include "Model3d.h"
#include "Projectile.h"
#include "ProjectileMaterial.h"
#include "Ray.h"
#include "SelectionMarkerMesh.h"
#include "Terrain.h"
#include "Vertex3d.h"
#include <array>
#include <vector>


Projectile::Projectile(Vector3 pos, Vector3 dir, Vector3 up, Entity* owner = nullptr) : Entity(pos, dir, up), owner(owner)
{
    if(!ModelManager::hasModel("projectile"))
    {
        depth = height = 0.03;
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
            triangles.insert(triangles.end(), { j, j+1, j+2 });
            triangles.insert(triangles.end(), { j, j+2, j+3 });
        }

        auto material = new ProjectileMaterial({ 1.0, 1.0, 0.0 });

        auto projectileMesh = new Mesh3d(vertices, triangles, material);
        /*projectileModel = new Model3d(*projectileMesh);
        Model3d::templateMap["projectile"] = projectileModel;
        for(auto mesh : projectileModel->meshes)
            mesh->isTemplate = true;*/
        ModelManager::addModel("projectile", *projectileMesh);
        projectileModel = ModelManager::instantiateModel("projectile");
    }
    else
        projectileModel = ModelManager::instantiateModel("projectile");

    setPosition(pos);
    setDirection(dir, up);
}

Projectile::~Projectile()
{
    ModelManager::deleteModel("projectile", projectileModel);
}

void Projectile::init(Scene* scene)
{
    this->scene = scene;
    projectileModel->setScene(scene);
}

void Projectile::setPosition(Vector3 pos)
{
    this->pos = pos;
    projectileModel->setPosition(pos);
}

void Projectile::setDirection(Vector3 dir, Vector3 up)
{
    this->dir = dir;
    projectileModel->setDirection(dir, up);
}

void Projectile::update(real dt)
{
    auto p1 = pos;
    for(auto unit : scene->getUnits())
    {
        if(unit->intersectBoundingBox(pos, pos+dt*velocity))
        {
            if(unit != owner)
            {
                auto [p, norm] = unit->getBoundingBoxIntersection(pos, pos+dt*velocity);
                for(int i = 0; i < 150; i++)
                {
                    auto gp = new UnitHitParticle(p, norm);
                    scene->addParticle(gp);
                }
                unit->health -= 20;
                if(unit->health < 0)
                    scene->removeUnit(unit);
            }
            scene->removeEntity(this);
        }
    }

    setPosition(pos + velocity*dt -Vector3(0, 0, gravity)*dt*dt*0.5f);
    setDirection(velocity.normalized(), ((velocity%up%velocity).normalized()));

    auto p2 = pos;
    auto v2 = scene->getTerrain()->intersect(Ray(pos, (p2-p1).normalized()), (p2-p1).length()*(1+1e-6));

    if(v2.length() < inf)
    {
        auto normal = scene->getTerrain()->getNormal(v2.x, v2.y);
        for(int i = 0; i < 250; i++)
        {
            auto gp = new GroundExplosionParticle(v2, normal);
            scene->addParticle(gp);
        }
        scene->removeEntity(this);
    }
    velocity += Vector3({ 0, 0, -gravity })*dt;
}

void Projectile::draw()
{
    projectileModel->updateUniforms();
    projectileModel->draw();
}
