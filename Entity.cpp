
#define NOMINMAX
#include "Entity.h"
#include "Mesh3d.h"
#include "Vertex3d.h"
#include "Model3d.h"
#include "LineMesh3d.h"
#include "Line.h"
#include "Ray.h"
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
    real w = 0.022, h = 0.022, d = 0.022;

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
    auto boundingBoxMesh = new Mesh3d(vertices, triangles, material);
    boundingBoxModel = new Model3d(*boundingBoxMesh);
    bbox = BoundingBox(Vector3(-w/2, -d/2, -h/2), Vector3(w/2, d/2, h/2));
    auto d1 = std::max(w, d);
    auto d2 = std::max(d, h);

    auto material2 = new LineMaterial({ 0, 0.8, 0.1 });
    std::vector<Vector3> vs({ { w/2, 0, 0 }, { (w*3.f)/4, 0, 0 }, { w*(3.f/4 - 1.f/10), w*1.f/10, 0 }, { w*(3.f/4 - 1.f/10), -w*1.f/10, 0 } });
    LineMesh3d* lineMesh = new LineMesh3d(vs, { { 0, 1 }, { 1, 2 }, { 1, 3 } }, material2);
    boundingBoxModel->addMesh(*lineMesh);
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
        std::cout << "Intersects" << std::endl;
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


Entity::~Entity()
{
}