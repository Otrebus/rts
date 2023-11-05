#include "Terrain.h"
#include "Utils.h"
#include "Mesh3d.h"
#include "Model.h"
#include "Vector3.h"
#include "TextureMaterial.h"


Terrain::Terrain(const std::string& fileName, Scene* scene) : scene(scene)
{
    auto [colors, width, height] = readBMP(fileName);
    std::vector<Vertex3d> vertices(width*height);
    std::vector<int> points;

    for(int y = height-1; y >= 0; y--)
    {
        for(int x = 0; x < width; x++)
        {
            real X = x/(width-1.0f);
            real Y = y/(height-1.0f);
            vertices[height*y+x] = Vertex3d(X, Y, colors[height*y+x].x/10 + 3.0, 0.0, 0.0, 1.0, X, Y);
        }
    }
    for(int y = 0; y < height-1; y++)
    {
        for(int x = 0; x < width-1; x++)
        {
            int a = height*y + x;
            int b = height*y + x+1;
            int c = height*(y+1) + x+1;
            int d = height*(y+1) + x;
            points.insert(points.end(), { a, b, c, a, c, d });
        }
    }
    
    TextureMaterial* mat = new TextureMaterial(fileName);
    auto terrainMesh = Mesh3d(vertices, points, mat);
    terrainModel = Model3d(terrainMesh);

    terrainModel.Setup(scene);
};


void Terrain::Draw()
{
    terrainModel.UpdateUniforms();
    terrainModel.Draw();
}