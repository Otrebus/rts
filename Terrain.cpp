#include "Terrain.h"
#include "Utils.h"
#include "TerrainMesh.h"
#include "Model3d.h"
#include "Vector3.h"
#include "TerrainMaterial.h"
#include "Math.h"
#include <array>


TerrainMesh* Terrain::createMesh(std::string fileName)
{
    auto [colors, width, height] = readBMP(fileName, false);
    std::vector<MeshVertex3d> vertices(width*height);
    points.clear();
    triangleIndices.clear();

    auto H = [&colors, &width, &height] (int x, int y) {
        return colors[width*3*(height-y-1)+x*3]/255.0*width/10;
    };

    for (int y = 0; y < height; y++)
    {
        for(int x = 0; x < width; x++)
        {
            real X = x;
            real Y = y;

            auto fx = x == 0, lx = x == width-1;
            auto fy = y == 0, ly = y == height-1;

            auto dx = ((!lx ? H(x+1, y) : H(x, y)) - (!fx ? H(x-1, y) : H(x, y)))/((!fx + !lx));
            auto dy = ((!ly ? H(x, y+1) : H(x, y)) - (!fy ? H(x, y-1) : H(x, y)))/((!fy + !ly));
            real l = std::sqrt(dx*dx+dy*dy+1);
            points.push_back(Vector3(X, Y, H(x, y) + 3.0));
            vertices[width*y+x] = MeshVertex3d(X, Y, H(x, y) + 3.0, -dx/l, -dy/l, 1.0/l, x, y);
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
                vertices[j++] = MeshVertex3d(vectors[i].x, vectors[i].y, vectors[i].z, N1.x, N1.y, N1.z, x, y);
                points.push_back({ vectors[i].x, vectors[i].y, vectors[i].z });
            }
            for(auto i : { a, c, d } ) {
                vertices[j++] = MeshVertex3d(vectors[i].x, vectors[i].y, vectors[i].z, N2.x, N2.y, N2.z, x, y);
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

real Terrain::getHeight(real x, real y) const
{
    int xl = x, yl = y;

    // TODO: could overflow, check
    auto p1 = points[yl*width+xl];
    auto p2 = points[yl*width+xl+1];
    auto p3 = points[(yl+1)*width+xl+1];
    auto p4 = points[(yl+1)*width+xl];

    auto dx = x-xl, dy = y-yl;
    Vector3 a, b, c;
    if(dy < dx)
        a = p1, b = p2, c = p3;
    else
        a = p1, b = p3, c = p4;

    auto v1 = c-a, v2 = b-a, w = Vector3(x, y, 0), A = Vector3(0, 0, -1);


    real z = v1*(v2%-(w-a))/(v1*(v2%-A));
    return z;
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
    return { { 0.f, 0.f, width/10.0f }, { real(width), real(height), width/10.0f } };
}


const Vector3& Terrain::getPoint(int x, int y) const
{
    return points[x + y*height];
}


bool Terrain::inBounds(int x, int y) const
{
    return x >= 0 && x < width && y >= 0 && y < height;
}


bool Terrain::isAdmissible(int x, int y) const
{
    if(!inBounds(x, y))
        return false;
    auto p = getPoint(x, y);
    std::array<std::pair<int, int>, 7> dirs = { { { x, y+1 }, { x-1, y }, { x-1, y-1 }, { x, y-1 }, { x+1, y }, { x+1, y+1 }, { x, y+1 } } };
    for(int i = 0; i < 6; i++) {
        auto [x1, y1] = dirs[i];
        auto [x2, y2] = dirs[i+1];

        if(!inBounds(x1, y1) || !inBounds(x2, y2))
            continue;

        auto p1 = getPoint(x1, y1);
        auto p2 = getPoint(x2, y2);

        auto cosSlope = std::abs((((p1-p)%(p2-p)).normalized()).z);
        if(cosSlope < cosMaxSlope)
            return false;
    }
    return true;
}


std::pair<int, int> Terrain::getClosestAdmissible(Vector2 v) const
{
    auto x = int(v.x), y = int(v.y);
    int y1, y2;
    int x1, x2;

    if(v.y > 0.5f+y)
    {
        if(v.x > 0.5f+x)
        {
            if(isAdmissible(x+1, y+1))
                return { x+1, y+1 };
            if(v.x-x > v.y-y && isAdmissible(x+1, y))
                return { x+1, y };
            if(isAdmissible(x, y+1))
                return { x, y+1 };
            return { x, y };
        }
        else
        {
            if(isAdmissible(x, y+1))
                return { x, y+1 };
            if(1-(v.x-x) > v.y-y && isAdmissible(x+1, y+1))
                return { x+1, y+1 };
            if(isAdmissible(x, y))
                return { x, y };
            return { x+1, y };
        }
    }
    else
    {
        if(v.x < 0.5f+x)
        {
            if(isAdmissible(x, y))
                return { x, y };
            if(v.x-x > v.y-y && isAdmissible(x+1, y))
                return { x+1, y };
            if(isAdmissible(x, y+1))
                return { x, y+1 };
            return { x+1, y+1 };
        }
        else
        {
            if(isAdmissible(x+1, y))
                return { x, y+1 };
            if(1-(v.x-x) > v.y-y && isAdmissible(x, y))
                return { x+1, y+1 };
            if(isAdmissible(x+1, y+1))
                return { x, y };
            return { x, y+1 };
        }
    }
    assert(false);
    return { x, y };
}


std::vector<Vector2> Terrain::findPath(Vector2 start, Vector2 destination)
{

}