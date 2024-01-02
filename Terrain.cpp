#include "Terrain.h"
#include "Utils.h"
#include "Mesh3d.h"
#include "Model.h"
#include "Vector3.h"
#include "TextureMaterial.h"
#include "LambertianMaterial.h"


Mesh3d Terrain::createMesh(std::string fileName)
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
    
    Material* mat = new LambertianMaterial(Vector3(0.5, 0.5, 0.5));
    auto terrainMesh = Mesh3d(vertices, points, mat);
    return terrainMesh;
}


Mesh3d Terrain::createFlatMesh(std::string fileName)
{
    auto [colors, width, height] = readBMP(fileName, false);
    std::vector<Vector3> vectors(width*height);

    const int nVertices = (width-1)*(height-1)*6;
    std::vector<Vertex3d> vertices(nVertices);
    std::vector<int> points(nVertices);

    auto H = [&colors, &width] (int x, int y) {
        return colors[width*3*y+x*3]/255.0/10;
    };

    for(int y = height-1; y >= 0; y--)
    {
        for(int x = 0; x < width; x++)
        {
            real X = x/(width-1.0f), Y = y/(height-1.0f);
            vectors[width*y+x] = Vector3(X, Y, H(x, y) + 3.0);
        }
    }

    int j = 0, k = 0;
    for(int y = 0; y < height-1; y++)
    {
        for(int x = 0; x < width-1; x++)
        {
            int a = width*y + x;
            int b = width*y + x+1;
            int c = width*(y+1) + x+1;
            int d = width*(y+1) + x;

            auto N1 = (vectors[b]-vectors[a])%(vectors[c]-vectors[a]);
            N1.normalize();
            auto N2 = (vectors[c]-vectors[a])%(vectors[d]-vectors[a]);
            N2.normalize();

            for(auto i : { a, b, c } )
                vertices[j++] = Vertex3d(vectors[i].x, vectors[i].y, vectors[i].z, N1.x, N1.y, N1.z, 0, 0);
            for(auto i : { a, c, d } )
                vertices[j++] = Vertex3d(vectors[i].x, vectors[i].y, vectors[i].z, N2.x, N2.y, N2.z, 0, 0);

            for(int i = j-6; i < j; i++)
                points[k++] = i;
        }
    }

    Material* mat = new LambertianMaterial(Vector3(0.5, 0.5, 0.5));
    auto terrainMesh = Mesh3d(vertices, points, mat);

    return terrainMesh;
}


Terrain::Terrain(const std::string& fileName, Scene* scene) : fileName(fileName), scene(scene)
{
    setUp();
};


void Terrain::setUp()
{
    auto terrainMesh = drawMode == DrawMode::Flat ? createFlatMesh(fileName) : createMesh(fileName);
    terrainModel = Model3d(terrainMesh);
    terrainModel.setup(scene);
}


void Terrain::tearDown()
{
    terrainModel.tearDown(scene);
}


void Terrain::draw()
{
    if(drawMode == DrawMode::Wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    terrainModel.updateUniforms();
    terrainModel.draw();
    if(drawMode != DrawMode::Normal)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Terrain::setDrawMode(DrawMode d)
{
    drawMode = d;
    std::cout << "Setting drawmode " << drawMode << std::endl;
    if(drawMode == DrawMode::Flat || drawMode == DrawMode::Normal) {
        tearDown();
        setUp();
    }
}


Terrain::DrawMode Terrain::getDrawMode() const
{
    return drawMode;
}