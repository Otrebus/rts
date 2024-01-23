#include "Entity.h"
#include "Mesh3d.h"
#include "Vertex3d.h"
#include "Model.h"
#include "LambertianMaterial.h"
#include <vector>
#include <array>


Vector3 calcNormal(Vector3 a, Vector3 b, Vector3 c)
{
    return ((c-b)%(a-b)).normalized();
}


Entity::Entity()
{
    int w, h, d;

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
    Model3d* model = new Model3d(boundingBoxMesh);
}


Entity::~Entity()
{
}