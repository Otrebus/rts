
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


Vector3 calcNormal(Vector3 a, Vector3 b, Vector3 c)
{
    return ((c-b)%(a-b)).normalized();
}


Entity::Entity(Vector3 pos, Vector3 dir, Vector3 up) : pos(pos), dir(dir), up(up)
{
    real w = 0.3, h = 0.3, d = 0.3;

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

    for(auto ci : cornerIndices) {
        int t1[3] = { 0, 1, 2 }, t2[3] = { 0, 2, 3 };

        int j = vertices.size();
        for(int i = 0; i < 4; i++)
            vertices.push_back({ c[ci[i]], calcNormal(c[ci[0]], c[ci[1]], c[ci[2]]), { 0, 0 } });
        triangles.insert(triangles.end(), { j, j+1, j+2 } );
        triangles.insert(triangles.end(), { j, j+2, j+3 } );
    }

    auto material = new LambertianMaterial({ 0, 0.8, 0.1 });
    boundingBoxMesh = new Mesh3d(vertices, triangles, material);
    boundingBoxModel = new Model3d(*boundingBoxMesh);
    bbox = BoundingBox(Vector3(-w/2, -d/2, -h/2), Vector3(w/2, d/2, h/2));
    auto d1 = std::max(w, d);
    auto d2 = std::max(d, h);

    auto material2 = new LineMaterial({ 0, 0.8, 0.1 });
    std::vector<Vector3> vs({ { w/2, 0, 0 }, { w, 0, 0 }, { (1.f - 0.25f)*w, w*0.25f, 0 }, { w*(1.0f - 0.25f), -w*0.25f, 0 }, { 0, 0, h/2 }, { 0, 0, h }, { 0, h*0.25f, (1.f - 0.25f)*h }, { 0, -h*0.25f, h*(1.0f - 0.25f) } });
    lineMesh = new LineMesh3d(vs, { { 0, 1 }, { 1, 2 }, { 1, 3 }, { 4, 5 }, { 5, 6 }, { 5, 7 } }, material2, 2);
    boundingBoxModel->addMesh(*lineMesh);
}


Entity::~Entity()
{
    //delete boundingBoxModel;
    //delete boundingBoxMesh;
    delete lineMesh;
}


void Entity::drawBoundingBox()
{
    boundingBoxModel->draw();
}

bool Entity::intersectBoundingBox(const Ray& ray)
{
    auto a = dir, b = up, c = dir%up, d = ray.dir, e = ray.pos-pos;

    auto det = a*(b%c);
    auto u2 = d*(b%c)/det;
    auto v2 = a*(d%c)/det;
    auto w2 = a*(b%d)/det;

    auto det2 = a*(b%c);
    auto u = e*(b%c)/det2;
    auto v = a*(e%c)/det2;
    auto w = a*(b%e)/det2;

    auto ray2 = Ray(Vector3(u, v, w), Vector3(u2, v2, w2));

    real tnear, tfar;
    if(bbox.intersect(ray2, tnear, tfar)) {
        return true;
    }
    return false;
}


void Entity::setSelected(bool selected)
{
    this->selected = selected;
    auto meshes = boundingBoxModel->getMeshes();
    auto& kd = ((LambertianMaterial*) meshes[0]->getMaterial())->Kd;
    kd = selected ? Vector3(0.8, 0, 0) : Vector3(0, 0.8, 0);
    auto& kd2 = ((LineMaterial*) meshes[1]->getMaterial())->Kd;
    kd2 = selected ? Vector3(0.8, 0, 0) : Vector3(0, 0.8, 0);
}


void Entity::setUp(Scene* scene)
{
    boundingBoxModel->setUp(scene);
}


void Entity::updateUniforms()
{
    boundingBoxModel->setPosition(pos);
    boundingBoxModel->setDirection(dir, up);
    boundingBoxModel->updateUniforms();
}


void Entity::plant(const Terrain& terrain)
{
    auto x = Vector3(dir.x, dir.y, 0).normalized();
    auto y = Vector3(-dir.y, dir.x, 0).normalized();
    auto z = Vector3(0, 0, 1);

    auto height = (bbox.c2.z-bbox.c1.z)/2;
    auto width = (bbox.c2.x-bbox.c1.x)/2;
    auto depth = (bbox.c2.y-bbox.c1.y)/2;

    auto a = pos + x*width + y*depth;
    auto b = pos - x*width + y*depth;
    auto c = pos - x*width - y*depth;
    auto d = pos + x*width - y*depth;

    auto ah = terrain.getHeight(a.x, a.y);
    auto bh = terrain.getHeight(b.x, b.y);
    auto ch = terrain.getHeight(c.x, c.y);
    auto dh = terrain.getHeight(d.x, d.y);

    Vector3 A = Vector3(a.x, a.y, ah);
    Vector3 B = Vector3(b.x, b.y, bh);
    Vector3 C = Vector3(c.x, c.y, ch);
    Vector3 D = Vector3(d.x, d.y, dh);

    up = ((C-B)%(A-B)).normalized();
    pos = Vector3(pos.x, pos.y, ((A+C)/2).z);
    auto l = Vector3(0, 0, 1)%x;
    dir = l%up;
    setPosition(pos);
    setDirection(dir.normalized(), up.normalized());
}


void Entity::setPosition(Vector3 pos)
{
    this->pos = pos;
}

void Entity::setDirection(Vector3 dir, Vector3 up)
{
    this->dir = dir;
    this->up = up;
}