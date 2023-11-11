#include "Terrain.h"
#include "Utils.h"
#include "Mesh3d.h"
#include "Model.h"
#include "Vector3.h"
#include "TextureMaterial.h"
#include "LambertianMaterial.h"


Terrain::Terrain(const std::string& fileName, Scene* scene) : scene(scene)
{
    auto [colors, width, height] = readBMP(fileName, false);
    std::vector<Vertex3d> vertices(width*height);
    std::vector<int> points;

    auto H = [&colors, &width] (int x, int y) {
        return colors[width*3*y+x*3]/255.0/10;
    };

    for(int y = height-1; y >= 0; y--)
    {
        for(int x = 0; x < width; x++)
        {
            real X = x/(width-1.0f);
            real Y = y/(height-1.0f);

            auto fx = x == 0, lx = x == width-1;
            auto fy = y == 0, ly = y == height-1;

            auto dx = ((!lx ? H(x+1, y) : H(x, y)) - (!fx ? H(x-1, y) : H(x, y)))/((!fx + !lx)*1./(width-1.));
            auto dy = ((!ly ? H(x, y+1) : H(x, y)) - (!fy ? H(x, y-1) : H(x, y)))/((!fy + !ly)*1./(height-1.));
            real l = std::sqrt(dx*dx+dy*dy+1);
            vertices[width*y+x] = Vertex3d(X, Y, H(x, y) + 3.0, -dx/l, -dy/l, 1.0/l, X, Y);
        }
    }
    for(int y = 0; y < height-1; y++)
    {
        for(int x = 0; x < width-1; x++)
        {
            int a = width*y + x;
            int b = width*y + x+1;
            int c = width*(y+1) + x+1;
            int d = width*(y+1) + x;
            points.insert(points.end(), { a, b, c, a, c, d });
        }
    }
    
    //TextureMaterial* mat = new TextureMaterial(fileName);
    Material* mat = new LambertianMaterial(Vector3(0.5, 0.5, 0.5));
    auto terrainMesh = Mesh3d(vertices, points, mat);
    terrainModel = Model3d(terrainMesh);

    terrainModel.Setup(scene);
};


void Terrain::Draw()
{
    terrainModel.UpdateUniforms();
    terrainModel.Draw();
}