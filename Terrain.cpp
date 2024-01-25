#include "Terrain.h"
#include "Utils.h"
#include "TerrainMesh.h"
#include "Model.h"
#include "Vector3.h"
#include "TerrainMaterial.h"
#include "Math.h"


TerrainMesh* Terrain::createMesh(std::string fileName)
{
    auto [colors, width, height] = readBMP(fileName, false);
    std::vector<MeshVertex3d> vertices(width*height);
    points.clear();
    triangleIndices.clear();

    auto H = [&colors, &width, &height] (int x, int y) {
        return colors[width*3*(height-y-1)+x*3]/255.0/10;
    };

    for (int y = 0; y < height; y++)
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
            points.push_back(Vector3(X, Y, H(x, y) + 3.0));
            vertices[width*y+x] = MeshVertex3d(X, Y, H(x, y) + 3.0, -dx/l, -dy/l, 1.0/l, X, Y);
            vertices[width*y+x].selected = false;
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
            triangleIndices.insert(triangleIndices.end(), { a, c, d, a, b, c });
        }
    }

    this->width = width;
    this->height = height;
    
    if(terrainMesh)
        delete terrainMesh;
    Material* mat = new TerrainMaterial();
    return terrainMesh = new TerrainMesh(vertices, triangleIndices, mat);
}


TerrainMesh* Terrain::createFlatMesh(std::string fileName)
{
    auto [colors, width, height] = readBMP(fileName, false);
    std::vector<Vector3> vectors(width*height);
    points.clear();
    triangleIndices.clear();

    const int nVertices = (width-1)*(height-1)*6;
    std::vector<MeshVertex3d> vertices(nVertices);

    auto H = [&colors, &width, &height] (int x, int y) {
        return colors[width*3*(height-y-1)+x*3]/255.0/10;
    };

    for(int y = 0; y < height; y++)
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
            {
                vertices[j++] = MeshVertex3d(vectors[i].x, vectors[i].y, vectors[i].z, N1.x, N1.y, N1.z, 0, 0);
                points.push_back({ vectors[i].x, vectors[i].y, vectors[i].z });
            }
            for(auto i : { a, c, d } ) {
                vertices[j++] = MeshVertex3d(vectors[i].x, vectors[i].y, vectors[i].z, N2.x, N2.y, N2.z, 0, 0);
                points.push_back({ vectors[i].x, vectors[i].y, vectors[i].z });
            }

            for(int i = j-6; i < j; i++)
                triangleIndices.push_back(i);
        }
    }

    this->width = width;
    this->height = height;

    if(terrainMesh)
        delete terrainMesh;
    Material* mat = new TerrainMaterial();
    return terrainMesh = new TerrainMesh(vertices, triangleIndices, mat);
}


void Terrain::selectTriangle(int i, bool selected)
{
    //int y = i/2/height;
    //int x = i/2%width;

    //int a = width*y + x;
    //int b = width*y + x+1;
    //int c = width*(y+1) + x+1;
    //int d = width*(y+1) + x;

    /*if(i%2 == 0)
    {*/
        terrainMesh->selectVertex(triangleIndices[i*3], selected);
        terrainMesh->selectVertex(triangleIndices[i*3+1], selected);
        terrainMesh->selectVertex(triangleIndices[i*3+2], selected);
    /*}
    else
    {
        terrainMesh->selectVertex(a, selected);
        terrainMesh->selectVertex(b, selected);
        terrainMesh->selectVertex(c, selected);
    }*/
}


void Terrain::intersect(const Ray& ray)
{
    real t1 = glfwGetTime();
    real closestT = inf;
    int closestIndex = -1;
    for(int i = 0; i < triangleIndices.size(); i += 3)
    {
        auto a = triangleIndices[i], b = triangleIndices[i+1], c = triangleIndices[i+2];
        auto [t, u, v] = intersectTriangle(points[a], points[b], points[c], ray);
        if(t > -inf && t < closestT)
            closestIndex = i/3, closestT = t;
    }
    if(closestIndex >= 0)
    {
        if(pickedTriangle >= 0)
            selectTriangle(pickedTriangle, false);
        pickedTriangle = closestIndex;
        selectTriangle(pickedTriangle, true);
    }
    real t2 = glfwGetTime();
    std::cout << (t2 - t1) << std::endl;
}


Terrain::Terrain(const std::string& fileName, Scene* scene) : fileName(fileName), scene(scene)
{
    setUp();
    pickedTriangle = -1;
};


void Terrain::setUp()
{
    auto terrainMesh = drawMode == DrawMode::Flat ? createFlatMesh(fileName) : createMesh(fileName);
    terrainModel = Model3d(*terrainMesh);
    terrainModel.setUp(scene);
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


std::pair<Vector3, Vector3> Terrain::getBoundingBox() const
{
    return { { 0, 0, 3 }, { 1, 1, 3 } };
}
