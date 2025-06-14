#include "GeometryUtils.h"
#include "Ray.h"
#include "Terrain.h"
#include "TerrainMaterial.h"
#include "TerrainMesh.h"
#include <array>
#include "FogOfWarMaterial.h"
#include "TexturedTerrainMaterial.h"
#include "ConsoleSettings.h"
#include <algorithm>
#include "PathFinding.h"

ConsoleVariable Terrain::fogOfWarEnabled("fogOfWar", 1);

Terrain::Terrain() : admissiblePoints(nullptr)
{
    fowMaterial = new FogOfWarMaterial();
    drawMode = Grid;
}

void Terrain::calcMinMax()
{
    max_R = new real[width];
    max_L = new real[width];
    min_R = new real[width];
    min_L = new real[width];

    for(int x = width-1; x >= 0; x--)
    {
        real max = x < width-1 ? max_R[x+1] : -inf;
        for(int y = 0; y < height; y++)
            max = std::max(max, points[y*width+x].z);
        max_R[x] = max;
    }

    for(int x = 0; x < width; x++)
    {
        real max = x ? max_L[x-1] : -inf;
        for(int y = 0; y < height; y++)
            max = std::max(max, points[y*width+x].z);
        max_L[x] = max;
    }

    for(int x = width-1; x >= 0; x--)
    {
        real min = x < width-1 ? min_R[x+1] : inf;
        for(int y = 0; y < height; y++)
            min = std::min(min, points[y*width+x].z);
        min_R[x] = min;
    }

    for(int x = 0; x < width; x++)
    {
        real min = x ? min_L[x-1] : inf;
        for(int y = 0; y < height; y++)
            min = std::min(min, points[y*width+x].z);
        min_L[x] = min;
    }
}

TerrainMesh* Terrain::createMesh(std::string fileName, bool textured = false)
{
    if(points.empty())
    {
        auto [colors, width, height] = readBMP(fileName, false);

        vertices.resize(width*height);

        auto H = [&colors, &width, &height](int x, int y)
        {
            return colors[width*3*(height-y-1)+x*3]/255.0f*width/15;
        };

        for(int y = 0; y < height; y++)
        {
            for(int x = 0; x < width; x++)
            {
                real X = real(x);
                real Y = real(y);

                auto fx = x == 0, lx = x == width-1;
                auto fy = y == 0, ly = y == height-1;

                auto dx = ((!lx ? H(x+1, y) : H(x, y)) - (!fx ? H(x-1, y) : H(x, y)))/((!fx + !lx));
                auto dy = ((!ly ? H(x, y+1) : H(x, y)) - (!fy ? H(x, y-1) : H(x, y)))/((!fy + !ly));
                real l = std::sqrt(dx*dx+dy*dy+1);
                points.push_back(Vector3(X, Y, H(x, y) + 3.0f));
                vertices[width*y+x] = MeshVertex3(X, Y, H(x, y) + 3.0f, -dx/l, -dy/l, 1.0f/l, X, Y);
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
        calcMinMax();
    }
    if(terrainMesh)
        delete terrainMesh;

    Material* mat = textured ? (Material*) new TexturedTerrainMaterial() : new TerrainMaterial();

    return terrainMesh = new TerrainMesh(vertices, triangleIndices, mat);
}


void Terrain::recalcAdmissiblePoint(int x, int y)
{
    admissiblePoints[width*y+x] = true;
    if(!isTriangleAdmissible(x, y, x+1, y, x+1, y+1) ||
       !isTriangleAdmissible(x-1, y-1, x, y-1, x, y) ||
       !isTriangleAdmissible(x-1, y, x, y, x, y+1) ||

       !isTriangleAdmissible(x, y, x+1, y+1, x, y+1) || 
       !isTriangleAdmissible(x, y-1, x+1, y, x, y) ||
       !isTriangleAdmissible(x-1, y-1, x, y, x-1, y)
    )
    {
        admissiblePoints[width*y+x] = false;
        return;
    }
    for(auto building : scene->getBuildings())
    {
        if(building->pointWithinFootprint(x, y))
        {
            admissiblePoints[width*y+x] = false;
            return;
        }
    }
}


void Terrain::calcAdmissiblePoints()
{
    for(int x = 0; x < width-1; x++)
    {
        for(int y = 0; y < height-1; y++)
        {
            auto p1 = getPoint(x, y);
            auto p2 = getPoint(x+1, y);
            auto p3 = getPoint(x+1, y+1);
            auto p = getPoint(x, y+1);
            if(!isTriangleAdmissible(x, y, x+1, y, x+1, y+1))
            {
                admissiblePoints[width*y+x] = false;
                admissiblePoints[width*y+x+1] = false;
                admissiblePoints[width*(y+1)+x+1] = false;
                
            }
            if(!isTriangleAdmissible(x, y, x+1, y+1, x, y+1))
            {
                admissiblePoints[width*y+x] = false;
                admissiblePoints[width*(y+1)+x+1] = false;
                admissiblePoints[width*(y+1)+x] = false;
            }
            for(auto building : scene->getBuildings())
            {
                for(int y = 0; y <= building->width; y++)
                {
                    for(int x = 0; x <= building->length; x++)
                    {
                        int X = int(building->pos.x + x);
                        int Y = int(building->pos.y + y);
                        if(building->pointWithinFootprint(X, Y))
                            admissiblePoints[width*y+x] = false;
                    }
                }
            }
        }
    }
    for(int x = 0; x < width-1; x++)
        for(int y = 0; y < height-1; y++)
            setAdmissible(x, y, admissiblePoints[width*y+x]);
}

void Terrain::updateAdmissiblePoints()
{
    std::lock_guard<std::mutex> guard(scene->buildingMutex);
    std::unordered_set<int> currentBuildingPoints;
    for(auto building : scene->getBuildings())
    {
        for(int y = 0; y <= building->width; y++)
        {
            for(int x = 0; x <= building->length; x++)
            {
                int X = int(building->pos.x + x - real(building->length)/2), Y = int(building->pos.y + y - real(building->width)/2);
                currentBuildingPoints.insert(Y*width+X);
            }
        }
    }

    for(auto p : buildingPoints)
        admissiblePoints[p] = true;
    for(auto p : currentBuildingPoints)
        admissiblePoints[p] = true;
    
    for(auto p : buildingPoints)
    {
        recalcAdmissiblePoint(p%width, p/width);
        setAdmissible(p%width, p/width, admissiblePoints[p]);
    }
    for(auto p : currentBuildingPoints)
    {
        recalcAdmissiblePoint(p%width, p/width);
        setAdmissible(p%width, p/width, admissiblePoints[p]);
    }
    buildingPoints = currentBuildingPoints;
}


Vector3 Terrain::intersectBrute(const Ray& ray, real maxT)
{
    auto minT = inf;
    Vector3 w = { inf, inf, inf };
    for(int X = 0; X < width - 1; X++)
    {
        for(int Y = 0; Y < height - 1; Y++)
        {
            auto p1 = getPoint(X, Y);
            auto p2 = getPoint(X+1, Y);
            auto p3 = getPoint(X+1, Y+1);
            auto p4 = getPoint(X, Y+1);

            auto [t, u, v] = intersectTriangle(p1, p2, p3, ray);
            if(t > 0 && t < minT && t < maxT)
            {
                w = ray.pos + ray.dir*real(t);
                minT = t;
            }

            auto [t2, u2, v2] = intersectTriangle(p1, p3, p4, ray);
            if(t2 > 0 && t2 < minT && t2 < maxT)
            {
                w = ray.pos + ray.dir*real(t2);
                minT = t2;
            }
        }
    }

    if(minT <= maxT)
        return w;
    return { inf, inf, inf };
}


Vector3 Terrain::intersectFast(const Ray& ray, real maxT)
{
    auto p = ray.pos, d = ray.dir;

    real x0, y0;

    if(p.x > 0 && p.x <= width-2 && p.y > 0 && p.y <= height-2)
    {
        x0 = p.x;
        y0 = p.y;
    }
    else
    {
        std::array<Vector2, 4> vs = {Vector2{0.f, 0.f}, Vector2{width - 2.0f, 0.f}, Vector2{width - 2.0f, height - 2.0f}, Vector2{0.f, height - 2.0f}};

        auto t = -inf;
        for(int i = 0; i < 4 && t == -inf; i++)
        {
            auto d2 = vs[(i+1)%4] - vs[i];
            auto t2 = -inf;
            if(d2.perp()*d.to2() > 0)
                t2 = intersectRaySegment(p.to2(), d.to2(), vs[i], vs[(i+1)%4]);
            if(t2 > 0)
                t = max(t, t2);
        }

        if(t == -inf)
            return { inf, inf, inf };
        auto p0 = p + d*t;
        x0 = p0.x;
        y0 = p0.y;
    }

    real xm = x0, xM, ym, yM;

    xM = std::trunc(xm+1);
    xm = std::trunc(xm);

    while(true)
    {
        if(xm < 0 || xm >= width-1)
            break;

        int X = int(xm);

        if(d.x != 0)
        {
            ym = p.y + d.y*(xm - p.x)/d.x;
            yM = p.y + d.y*(xM - p.x)/d.x;

            if(d.x > 0)
            {
                auto z = (d.z/d.x)*(X-p.x) + p.z;
                if(d.z > 0 && z > max_R[X+1])
                    return { inf, inf, inf };
                if(d.z < 0 && z < min_R[X+1])
                    return { inf, inf, inf };
            }
            else
            {
                auto z = (d.z/d.x)*(X+1-p.x) + p.z;
                if(d.z > 0 && z > max_L[X])
                    return { inf, inf, inf };
                if(d.z < 0 && z < min_L[X])
                    return { inf, inf, inf };
            }
        }
        else
        {
            ym = 0.f;
            yM = real(height);
        }

        auto yMin = std::max(0.f, std::min(ym, yM));
        auto yMax = std::min(height-1, int(std::max(ym, yM)+1));

        auto yStart = d.y > 0 ? yMin : yMax - 1;
        auto yEnd = d.y > 0 ? yMax : yMin;

        int dy = d.y > 0 ? 1 : -1;

        for(int Y = int(yStart); Y != yEnd && Y >= 0 && Y < height; Y += dy)
        {
            auto p1 = getPoint(X, Y);
            auto p2 = getPoint(X+1, Y);
            auto p3 = getPoint(X+1, Y+1);
            auto p4 = getPoint(X, Y+1);

            auto [t, u, v] = intersectTriangle(p1, p2, p3, ray);
            auto [t2, u2, v2] = intersectTriangle(p1, p3, p4, ray);

            // TODO: run tests against this
            if(dy > 0 && Y - ray.pos.y > maxT || dy < 0 && (ray.pos.y - (Y+1)) > maxT)
                break;

            t = (t == -inf || t > maxT) ? inf : t;
            t2 = (t2 == -inf || t2 > maxT) ? inf : t2;

            if(t < t2)
                return ray.pos + ray.dir*real(t);
            else if(t2 != inf)
                return ray.pos + ray.dir*real(t2);
        }
        if(d.x > 0)
            xm = xM++;
        else if(d.x < 0)
            xM = xm--;
        else
            break;
    }
    return { inf, inf, inf };
}

Vector3 Terrain::intersect(const Ray& ray, real maxT)
{
    return intersectFast(ray, maxT);
}

Terrain::Terrain(const std::string& fileName, Scene* scene) : fileName(fileName), scene(scene), drawMode(Grid)
{
    init();
    scene->setTerrain(this);
    fowMaterial = new FogOfWarMaterial();
};

void Terrain::init()
{
    if(drawMode == Normal)
        createMesh(fileName, true);
    else
        createMesh(fileName, false);
    terrainModel = new Model3d(*terrainMesh);
    terrainModel->init();
    terrainModel->setScene(scene);

    glGenBuffers(1, &fogBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, fogBuffer);

    std::vector<int> fogData(2+width*height, 1);
    fogOfWar = std::vector<int>(width*height, 1);

    fogData[0] = width;
    fogData[1] = height;

    glBufferData(GL_SHADER_STORAGE_BUFFER, fogData.size() * sizeof(int), fogData.data(), GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, fogBuffer);

    glGenBuffers(1, &admissibleBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, admissibleBuffer);

    std::vector<int> admissibleData(2+width*height, 1);

    admissibleData[0] = width;
    admissibleData[1] = height;

    glBufferData(GL_SHADER_STORAGE_BUFFER, admissibleData.size() * sizeof(int), admissibleData.data(), GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, admissibleBuffer);

    admissiblePoints = new bool[width*height];
    std::fill(admissiblePoints, admissiblePoints + width*height, true);
    calcAdmissiblePoints();
}

void Terrain::tearDown()
{
    terrainModel->tearDown(scene);
}

void Terrain::draw()
{
    if(drawMode == DrawMode::Wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    if(drawMode != DrawMode::Normal)
        ((TerrainMesh*)(terrainModel->getMeshes()[0]))->setFlat(drawMode == DrawMode::Flat);
    terrainModel->updateUniforms();
    terrainModel->draw();

    if(fogOfWarEnabled.varInt())
    {
        GLint curDepthFun;
        GLboolean curBlend;

        GLint curSrc, curDst;

        glGetIntegerv(GL_DEPTH_FUNC, &curDepthFun);
        glGetBooleanv(GL_BLEND, &curBlend);
        glGetIntegerv(GL_BLEND_SRC_RGB, &curSrc);
        glGetIntegerv(GL_BLEND_DST_RGB, &curDst);

        glEnable(GL_BLEND);
        //glBlendFunc(GL_ZERO, GL_SRC_COLOR);
        glBlendFuncSeparate(GL_ZERO, GL_SRC_COLOR, GL_ONE, GL_ZERO);
        glDepthFunc(GL_LEQUAL);
        terrainModel->draw(fowMaterial);
        if(!curBlend)
            glDisable(GL_BLEND);
        glDepthFunc(curDepthFun);
        glBlendFunc(curSrc, curDst);
    }

    if(drawMode != DrawMode::Grid)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

real Terrain::getElevation(real x, real y) const
{
    if(x == int(x) && y == int(y))
        return points[int(y)*width+int(x)].z;
    // TODO: could overflow, check
    // temporary (ha!) hack
    if(y > 0.f)
        y -= 1e-4f;
    if(x > 0.f)
        x -= 1e-4f;

    int xl = int(x), yl = int(y);
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

Vector3 Terrain::getNormal(real x, real y) const
{
    // TODO: could overflow, check
    // temporary hack
    if(y > 0)
        y -= 1e-4f;
    if(x > 0)
        x -= 1e-4f;

    int xl = int(x), yl = int(y);
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

    return (v2%v1).normalized();
}

void Terrain::setDrawMode(DrawMode d)
{
    drawMode = d;
    if(drawMode == DrawMode::Normal || drawMode == DrawMode::Grid)
    {
        tearDown();
        init();
    }
}

Terrain::DrawMode Terrain::getDrawMode() const
{
    return drawMode;
}

void Terrain::setAdmissible(int x, int y, bool b)
{
    admissiblePoints[y*width+x] = b;
    int data = b;
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, admissibleBuffer);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, sizeof(int)*(2+y*width+x), 4, &data);
}

void Terrain::setFog(int x, int y, bool b)
{
    fogOfWar[y*width+x] = b;
    int data = b;
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, fogBuffer);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, sizeof(int)*(2+y*width+x), 4, &data);
}

bool Terrain::getFog(int x, int y) const
{
    if(x < 0 || x >= width || y < 0 || y >= height)
        return true;
    return fogOfWar[y*width+x];
}

std::pair<Vector3, Vector3> Terrain::getBoundingBox() const
{
    return { { 0.f, 0.f, width/10.f }, { real(width), real(height), width/10.f } };
}

const Vector3& Terrain::getPoint(int x, int y) const
{
    assert(inBounds(x, y));
    return points[x + y*width];
}

void Terrain::setElevation(int x, int y, real h)
{
    int p = x + y*width;
    points[p].z = h;

    // TODO: update normal
    ((TerrainMesh*)(terrainModel->getMeshes()[0]))->updateElevation(p, h);
    vertices[x + y*width].pos.z = h;

    // TODO: update path finding points
}

bool Terrain::inBounds(int x, int y) const
{
    return x >= 0 && x < width && y >= 0 && y < height;
}

bool Terrain::isAdmissible(int x, int y) const
{
    return admissiblePoints[y*width+x];
}

std::pair<int, int> Terrain::getClosestAdmissible(Vector2 v) const
{
    auto x = int(v.x), y = int(v.y);

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
                return { x+1, y };
            return { x, y+1 };
        }
    }
}

bool Terrain::isTriangleAdmissible(const Vector3& p1, const Vector3& p2, const Vector3& p3) const
{
    auto cosSlope = std::abs((((p2-p1)%(p3-p1)).normalized()).z);
    return cosSlope >= cosMaxSlope;
}

bool Terrain::isTriangleAdmissible(int x1, int y1, int x2, int y2, int x3, int y3) const
{
    if(x1 >= width || x2 >= width || x3 >= width || x1 < 0 || x2 < 0 || x3 < 0)
        return false;
    if(y1 >= height || y2 >= height || y3 >= height || y1 < 0 || y2 < 0 || y3 < 0)
        return false;

    return isTriangleAdmissible(points[y1*width+x1], points[y2*width+x2], points[y3*width+x3]);
}

std::pair<real, Vector2> Terrain::intersectRayOcclusion(Vector2 pos, Vector2 dir) const
{
    auto p = pos;
    int x = int(p.x), y = int(p.y);
    int X = x, Y = y;
    int dx = x-X, dy = y-Y;

    auto v = dir, w = Vector2(1, 1);
    real s = (w.x*p.y - p.x*w.y)/(-w.x*v.y+v.x*w.y);
    real t = inf;

    auto t1 = (1-(p.y-y))/v.y;
    auto t2 = -(p.y-y)/v.y;
    auto t3 = (1-(p.x-x))/v.x;
    auto t4 = -(p.x-x)/v.x;

    if(t1 > eps && t1 < t)
        t = t1;

    if(t2 > eps && t2 < t)
        t = t2;

    if(t3 > eps && t3 < t)
        t = t3;

    if(t4 > eps && t4 < t)
        t = t4;

    if(t > 0 && s < t)
    {
        if(dy < dx && !isTriangleAdmissible(X, Y, X, X+1, X+1, Y+1))
            return { t, { -real(dx), real(dy) } };
        if(dx < dy && !isTriangleAdmissible(X, Y, X, X+1, X+1, Y+1))
            return { t, { real(dx), -real(dy) } };
    }

    if(t == t1 && !isTriangleAdmissible(X, Y+1, X+1, Y+1, X+1, Y+2))
        return { t, { 0, -1 } };
    if(t == t2 && !isTriangleAdmissible(X, Y, X+1, Y, X, Y-1))
        return { t, { 0, 1 } };
    if(t == t3 && isTriangleAdmissible(X+1, Y, X+1, Y+1, X+2, Y))
        return { t, { -1, 0 } };
    if(t == t4 && !isTriangleAdmissible(X, Y, X-1, Y, X, Y+1))
        return { t, { 1, 0 } };

    if(t < inf)
    {
        auto [u, V] = intersectRayOcclusion(pos+dir*t, dir);
        return { u + t, V };
    }
    return { -inf, { 0, 0 } };
}

bool Terrain::isTriangleAdmissible(Vector2 p) const
{
    int x = int(p.x), y = int(p.y);
    if(p.x-x > p.y-y)
        return isTriangleAdmissible(x, y, x+1, y, x+1, y+1);
    return isTriangleAdmissible(x, y, x+1, y+1, x, y+1);
}

std::pair<real, Vector2> Terrain::intersectCirclePathOcclusion(Vector2 pos, Vector2 pos2, real radius) const
{
    auto v = pos2 - pos;
    if(!v)
        return { -inf, { 0, 0 } };

    // auto w = v.perp().normalized()*radius;

    // For longer travels we might want to do do something like this involving edge lists and stuff to
    // figure out what quads to intersect with
    //Vector2 V[4] = { pos + w, pos + w + v, pos - w, pos - w + v };

    //auto y1 = std::min_element(V, V+4, [] (auto u, auto v) { return u.y < v.y; })->y;
    //auto y2 = std::max_element(V, V+4, [] (auto u, auto v) { return u.y < v.y; })->y;

    //y1 = int(std::ceil(y1)+eps);
    //y2 = int(y2);

    //std::vector<int> L(y2-y1+1, 0);
    //std::vector<int> R(y2-y1+1, 0);

    //for(int i = 0; i < 4; i++)
    //{
    //    auto a = V[(i+1)%4], b = V[i];
    //    auto u = a - b;

    //    auto ym = std::min(a.y, b.y), yM = std::max(a.y, b.y);
    //    auto xm = std::min(a.x, b.x), xM = std::max(a.x, b.x);

    //    for(int y = std::ceil(ym+eps); y <= int(yM); y++)
    //    {
    //        auto dy = yM - ym;
    //        auto dx = xM - xm;
    //        auto x = xm + (dx/dy)*(y-ym);
    //        L[y-y1] = std::min(int(x), L[y-y1]);
    //        R[y-y1] = std::max(int(std::ceil(x)+eps), L[y-y1]);

    //        auto t1 = intersectSphereTrianglePath(pos, radius, (pos2-pos).normalized(), getPoint(x, y).to2(), getPoint(x, y-1).to2(), getPoint(x+1, y).to2());
    //        auto t2 = intersectSphereTrianglePath(pos, radius, (pos2-pos).normalized(), getPoint(x+1, y).to2(), getPoint(x, y-1).to2(), getPoint(x+1, y+1).to2());
    //    }
    //}
    int x = int(pos.x), y = int(pos.y);

    auto mint = inf;
    Vector2 N;

    // TODO: we probably only want to trace in the direction towards pos2
    for(int dx = -3; dx <= 3; dx++)
    {
        for(int dy = -3; dy <= 3; dy++)
        {
            int X = x+dx, Y = y+dy;

            auto getSentineledPoint = [this] (int x, int y)
            {
                if(!inBounds(x, y))
                {
                    return Vector3(real(x), real(y), 1e3f);
                }
                return getPoint(x, y);
            };

            auto p1 = getSentineledPoint(X, Y);
            auto p2 = getSentineledPoint(X+1, Y);
            auto p3 = getSentineledPoint(X+1, Y+1);
            auto p4 = getSentineledPoint(X, Y+1);

            if(!isTriangleAdmissible(p1, p2, p3))
            {
                auto [t, norm] = intersectCircleTrianglePath(pos, radius, (pos2-pos).normalized(), p1.to2(), p2.to2(), p3.to2());
                if(t > -inf && t < mint)
                    mint = t, N = norm;
            }
            if(!isTriangleAdmissible(p1, p3, p4))
            {
                auto [t, norm] = intersectCircleTrianglePath(pos, radius, (pos2-pos).normalized(), p1.to2(), p3.to2(), p4.to2());
                if(t > -inf && t < mint)
                    mint = t, N = norm;
            }
        }
    }
    return { mint, N };
}

bool Terrain::isVisible(Vector2 start, Vector2 end) const
{
    auto dx = end.x-start.x;
    auto dy = end.y-start.y;
    if(!dx && !dy)
        return true;
    if(std::abs(dx) > std::abs(dy))
    {
        if(dx < 0)
            return isVisible(end, start);
        for(int x = int(std::ceil(start.x)); x <= int(std::floor(end.x)); x++)
        {
            auto y = start.y + (dy/dx)*(x-start.x);
            int Y = int(y + 0.5f);
            int X = x;
            if(!isAdmissible(X, Y))
                return false;
        }
    }
    else
    {
        if(dy < 0)
            return isVisible(end, start);
        for(int y = int(std::ceil(start.y)); y <= int(std::floor(end.y)); y++)
        {
            auto x = start.x + (dx/dy)*(y-start.y);
            int X = int(x + 0.5f);
            int Y = y;
            if(!isAdmissible(X, Y))
                return false;
        }
    }
    return true;
}

std::vector<Vector3> Terrain::chopLine(Vector2 start, Vector2 end) const
{
    std::vector<Vector3> result = { { start.x, start.y, getElevation(start.x, start.y) } };
    auto dx = end.x-start.x;
    auto dy = end.y-start.y;
    if(!dx && !dy)
        return {};
    if(std::abs(dx) > std::abs(dy))
    {
        if(dx < 0)
        {
            auto t = chopLine(end, start);
            std::reverse(t.begin(), t.end());
            return t;
        }
        for(int x = int(std::ceil(start.x)); x <= int(std::floor(end.x)); x++)
        {
            auto y = start.y + (dy/dx)*(x-start.x);
            result.push_back( { real(x), y, getElevation(real(x), real(y)) } );
        }
    }
    else
    {
        if(dy < 0)
        {
            auto t = chopLine(end, start);
            std::reverse(t.begin(), t.end());
            return t;
        }
        for(int y = int(std::ceil(start.y)); y <= int(std::floor(end.y)); y++)
        {
            auto x = start.x + (dx/dy)*(y-start.y);
            result.push_back( { x, real(y), getElevation(x, real(y)) } );
        }
    }
    result.push_back( { end.x, end.y, getElevation(end.x, end.y) });
    return result;
}

int Terrain::getHeight() const
{
    return height;
}

int Terrain::getWidth() const
{
    return width;
}

Scene* Terrain::getScene() const
{
    return scene;
}
