#include "BoundingBoxModel.h"
#include "LambertianMaterial.h"
#include "LineMaterial.h"
#include "LineMesh3d.h"
#include "Math.h"


BoundingBoxModel::BoundingBoxModel(real width, real depth, real height)
{
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

    auto material = new LambertianMaterial({ 0, 0.8, 0.1 });

    Mesh3d* boundingBoxMesh;

    boundingBoxMesh = new Mesh3d(vertices, triangles, material);
    addMesh(*boundingBoxMesh);
    auto boundingBox = BoundingBox(Vector3(-w/2, -d/2, -h/2), Vector3(w/2, d/2, h/2));
    auto d1 = std::max(w, d);
    auto d2 = std::max(d, h);

    auto material2 = new LineMaterial({ 0, 0.8, 0.1 });
    std::vector<Vector3> vs({ { w/2, 0, 0 }, { w, 0, 0 }, { (1.f - 0.25f)*w, w*0.25f, 0 }, { w*(1.0f - 0.25f), -w*0.25f, 0 }, { 0, 0, h/2 }, { 0, 0, h }, { 0, h*0.25f, (1.f - 0.25f)*h }, { 0, -h*0.25f, h*(1.0f - 0.25f) } });
    auto lineMesh = new LineMesh3d(vs, { { 0, 1 }, { 1, 2 }, { 1, 3 }, { 4, 5 }, { 5, 6 }, { 5, 7 } }, material2, 2);
    addMesh(*lineMesh);
}

BoundingBoxModel::~BoundingBoxModel()
{
}