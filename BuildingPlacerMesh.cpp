#include "LambertianMaterial.h"
#include "BuildingPlacerMesh.h"
#include "Tank.h"
#include "Terrain.h"
#include "BuildingPlacerMaterial.h"

BuildingPlacerMesh::BuildingPlacerMesh(Scene* scene, int width, int height) : width(width), height(height)
{
    this->scene = scene;
}

void BuildingPlacerMesh::init()
{
    auto [vs, triangles] = calcVertices(scene, 0, 0);

    auto material = new BuildingPlacerMaterial({ 0.f, 0.8f, 0.1f });

    v = vs;
    this->triangles = triangles;
    this->material = material;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex3)*v.size(), v.data(), GL_STREAM_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*triangles.size(), triangles.data(), GL_STREAM_DRAW);
}

std::pair<std::vector<Vertex3>, std::vector<int>> BuildingPlacerMesh::calcVertices(Scene* scene, int xpos, int ypos)
{
    std::vector<Vertex3> vs;
    for(int y = 0; y <= height; y++)
    {
        for(int x = 0; x <= width; x++)
        {
            auto X = float(xpos)+x, Y = float(ypos)+y;
            auto pos = scene->getTerrain()->getPoint(int(X), int(Y));
            pos.z += 0.01f;
            vs.push_back({ pos.x, pos.y, pos.z, 0.f, 0.f, 1.f, real(pos.x), real(pos.y) });
        }
    }

    std::vector<int> triangles;

    for(int y = 0; y < height; y++)
    {
        for(int x = 0; x < width; x++)
        {
            int i1 = y*(width+1)+x, i2 = y*(width+1)+x+1, i3 = (y+1)*(width+1)+x+1, i4 = (y+1)*(width+1) + x;
            triangles.insert(triangles.end(), { i1, i3, i2, i1, i4, i3 });
        }
    }
    return { vs, triangles };
}

void BuildingPlacerMesh::update(int xpos, int ypos, bool allowed)
{
    auto [vs, triangles] = calcVertices(scene, xpos, ypos);
    v = vs;
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex3)*v.size(), v.data(), GL_STREAM_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*triangles.size(), triangles.data(), GL_STREAM_DRAW);

    ((BuildingPlacerMaterial*)material)->Kd = allowed ? Vector3(0.f, 0.8f, 0.1f) : Vector3(0.8f, 0.f, 0.1f);
    nTriangles = int(triangles.size());
}
