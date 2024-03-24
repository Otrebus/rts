#include "Terrain.h"
#include "Utils.h"
#include "TerrainMesh.h"
#include "Model3d.h"
#include "Vector3.h"
#include "TerrainMaterial.h"
#include "Math.h"
#include <array>
#include "Ray.h"
#include <set>
#include <algorithm>
#include "Math.h"


TerrainMesh* Terrain::createMesh(std::string fileName)
{
    auto [colors, width, height] = readBMP(fileName, false);
    std::vector<MeshVertex3d> vertices(width*height);
    points.clear();
    triangleIndices.clear();

    auto H = [&colors, &width, &height] (int x, int y) {
        return colors[width*3*(height-y-1)+x*3]/255.0*width/15;
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

    admissiblePoints = new bool[width*height*2];
    std::fill(admissiblePoints, admissiblePoints + width*height*2, true);
    calcAdmissiblePoints();
    
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
    std::vector<int> triangleIndices2;

    const int nVertices = width*height;
    std::vector<MeshVertex3d> vertices(nVertices);

    auto H = [&colors, &width, &height] (int x, int y) {
        return colors[width*3*(height-y-1)+x*3]/255.0*width/15;
    };

    for(int y = 0; y < height; y++)
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
        }
    }
    
    std::vector<MeshVertex3d> vertices2;
    int j = 0, k = 0;
    for(int y = 0; y < height-1; y++)
    {
        for(int x = 0; x < width-1; x++)
        {
            int a = width*y + x;
            int b = width*y + x+1;
            int c = width*(y+1) + x+1;
            int d = width*(y+1) + x;

            auto N1 = (points[b]-points[a])%(points[c]-points[a]);
            N1.normalize();
            auto N2 = (points[c]-points[a])%(points[d]-points[a]);
            N2.normalize();

            for(auto i : { a, b, c } )
            {
                triangleIndices2.push_back(vertices2.size());
                triangleIndices.push_back(i);
                vertices2.push_back(vertices[i]);
                vertices2.back().normal = N1;
                if(!isTriangleAdmissible(points[a], points[b], points[c]))
                    vertices2.back().selected = true;
            }
            for(auto i : { a, c, d } ) {
                triangleIndices2.push_back(vertices2.size());
                triangleIndices.push_back(i);
                vertices2.push_back(vertices[i]);
                vertices2.back().normal = N2;
                if(!isTriangleAdmissible(points[a], points[c], points[d]))
                    vertices2.back().selected = true;
            }
        }
    }

    this->width = width;
    this->height = height;

    admissiblePoints = new bool[width*height*2];
    std::fill(admissiblePoints, admissiblePoints + width*height*2, true);
    calcAdmissiblePoints();

    if(terrainMesh)
        delete terrainMesh;
    Material* mat = new TerrainMaterial();
    return terrainMesh = new TerrainMesh(vertices2, triangleIndices2, mat);
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
        }
    }
}


Vector3 Terrain::intersect(const Ray& ray)
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
    if(closestT < inf)
        return ray.pos + ray.dir*closestT;
    return { inf, inf, inf };
}


Terrain::Terrain(const std::string& fileName, Scene* scene) : fileName(fileName), scene(scene)
{
    setUp();
    pickedTriangle = -1;
    scene->setTerrain(this);
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

real Terrain::getElevation(real x, real y) const
{
    // TODO: could overflow, check
    // temporary hack
    if(y > 0)
        y -= 1e-4;
    if(x > 0)
        x -= 1e-4;

    int xl = x, yl = y;
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
    return admissiblePoints[y*width+x];
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
    assert(false);
    return { x, y };
}


std::vector<Vector2> Terrain::findPath(Vector2 start, Vector2 destination)
{
    auto time = glfwGetTime();

    std::vector<bool> V(width*height, false);
    std::vector<real> C(width*height, inf);
    std::vector<std::pair<int, int>> P(width*height);

    std::set<std::pair<real, std::pair<int, int>>> Q;

    auto [startX, startY] = getClosestAdmissible(start);
    auto [destX, destY] = getClosestAdmissible(destination);

    int startIndex = startY*width + startX;
    int endIndex = destY*width + destX;
    C[startIndex] = 0;

    Q.insert({ 0.f, { startX, startY } });
    while(!Q.empty())
    {
        auto it = Q.begin();
        auto [prio, node] = *it;
        auto [x, y] = node;
        Q.erase(it);
        auto i = x + y*width;
        V[i] = true;
        if(i == endIndex)
            break;

        for(int dx = -1; dx <= 1; dx++)
        {
            for(int dy = -1; dy <= 1; dy++)
            {
                int j = (y+dy)*width + x+dx;
                if((dx || dy) && inBounds(x+dx, y+dy) && !V[j] && isAdmissible(x+dx, y+dy))
                {
                    int hx = (destY-(y+dy));
                    int hy = (destX-(x+dx));
                    real h = std::sqrt(hx*hx + hy*hy);
                    if(auto c2 = C[i] + std::sqrt(real(dx*dx+dy*dy)); c2 < C[j])
                    {
                        P[(y+dy)*width+x+dx] = { x, y };
                        Q.erase({ C[j]+h, { x+dx, y+dy } });
                        Q.insert({ c2 + h, { x+dx, y+dy } });
                        C[j] = c2;
                    }
                }
            }
        }
    };

    std::vector<Vector2> outPath = {};
    if(C[destX+destY*width] < inf)
    {
        std::vector<Vector2> result;

        for(std::pair<int, int> node = { destX, destY };;)
        {
            auto [x, y] = node;
            result.push_back({ real(x), real(y) });
            if(x == startX && y == startY)
                break;
            node = P[x+y*width];
        }
        if(result.size())
            result.back() = start;
        outPath = straightenPath(result);
        //outPath = result;
        std::cout << "Constructed path in " << glfwGetTime() - time << std::endl;
    }

    return outPath;
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
    int x = p.x, y = p.y;
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

    if(t == t1)
    {
        if(!isTriangleAdmissible(X, Y+1, X+1, Y+1, X+1, Y+2))
            return { t, { 0, -1 }};
    }
    if(t == t2)
    {
        if(!isTriangleAdmissible(X, Y, X+1, Y, X, Y-1))
            return { t, { 0, 1 }};
    }
    if(t == t3)
    {
        if(!isTriangleAdmissible(X+1, Y, X+1, Y+1, X+2, Y))
            return { t, { -1, 0 }};
    }
    if(t == t4)
    {
        if(!isTriangleAdmissible(X, Y, X-1, Y, X, Y+1))
            return { t, { 1, 0 }};
    }

    if(t < inf) {
        auto [u, V] = intersectRayOcclusion(pos+dir*t, dir);
        return { u + t, V };
    }
    return { -inf, { 0, 0 } };
}

real intersectRayCircle(Vector2 pos, Vector2 dir, Vector2 c, real radius)
{
    auto p0 = pos-c, p1 = dir;

    auto C = (p0.length2() - radius*radius)/p1.length2();
    auto B = 2*(p0.x*p1.x + p0.y*p1.y)/p1.length2();

    auto s = std::sqrt(B*B/4 - C);
    if(B*B/4 - C < 0)
        return -inf;
    auto t1 = -B/2 - s, t2 = -B/2 + s;

    return t1 > 0 ? t1 : t2 > 0 ? t2 : -inf;
}

real intersectRaySegment(Vector2 pos, Vector2 dir, Vector2 p1, Vector2 p2)
{
    auto e = p2 - p1;
    auto c = pos - p1;
    auto det = (-dir.x*e.y + e.x*dir.y);

    auto s = (c.x*e.y - e.x*c.y)/det;
    auto t = (-dir.x*c.y + c.x*dir.y)/det;
    if(t >= 0 && t <= 1 && s >= 0)
        return s;
    return -inf;
}


std::tuple<real, Vector2, real> distPointLine(Vector2 p, Vector2 p1, Vector2 p2)
{
    auto e = (p2-p1).normalized();
    auto s = (p-p1)*e;
    return { (p - (p1 + e*s)).length(), p1 + e*s, s };
}


std::pair<real, Vector2> intersectCircleTrianglePath(Vector2 pos, real radius, Vector2 dir, Vector2 p1, Vector2 p2, Vector2 p3)
{
    auto pp1 = (p1-p2).perp().normalized(), pp2 = (p2-p3).perp().normalized(), pp3 = (p3-p1).perp().normalized();
    auto e11 = p1 + pp1*radius, e12 = p2 + pp1*radius;
    auto e21 = p2 + pp2*radius, e22 = p3 + pp2*radius;
    auto e31 = p3 + pp3*radius, e32 = p1 + pp3*radius;

    // First check if we're closer than the radius of the circle to the triangle
    if(auto [t, q, s] = distPointLine(pos, p1, p2); t < radius && s > 0 && s < (p2-p1).length())
        return { 0, (p1-q).perp().normalized() };

    if(auto [t, q, s] = distPointLine(pos, p2, p3); t < radius && s > 0 && s < (p3-p2).length())
        return { 0, (p2-q).perp().normalized() };

    if(auto [t, q, s] = distPointLine(pos, p3, p1); t < radius && s > 0 && s < (p1-p3).length())
        return { 0, (p3-q).perp().normalized() };

    if(auto t = (pos - p1).length(); t < radius)
        return { 0, (pos - p1).normalized() };

    if(auto t = (pos - p2).length(); t < radius)
        return { 0, (pos - p2).normalized() };

    if(auto t = (pos - p3).length(); t < radius)
        return { 0, (pos - p3).normalized() };

    real minT = inf;
    Vector2 norm;

    real t = intersectRaySegment(pos, dir, e11, e12);
    if(t > 0)
        minT = t, norm = pp1;

    t = intersectRaySegment(pos, dir, e21, e22);
    if(t > 0 && t < minT)
        minT = t, norm = pp2;

    t = intersectRaySegment(pos, dir, e31, e32);
    if(t > 0 && t < minT)
        minT = t, norm = pp3;

    t = intersectRayCircle(pos, dir, p1, radius);

    // TODO: we probably want different normals here
    if(t > 0 && t < minT)
        minT = t, norm = (pos+dir*t-p1).normalized();

    t = intersectRayCircle(pos, dir, p2, radius);
    if(t > 0 && t < minT)
        minT = t, norm = (pos+dir*t-p2).normalized();

    t = intersectRayCircle(pos, dir, p3, radius);
    if(t > 0 && t < minT)
        minT = t, norm = (pos+dir*t-p3).normalized();

    return { minT, norm };
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

    for(int dx = -1; dx <= 1; dx++)
    {
        for(int dy = -1; dy <= 1; dy++)
        {
            int X = x+dx, Y = y+dy;
            auto p1 = getPoint(X, Y), p2 = getPoint(X+1, Y), p3 = getPoint(X+1, Y+1), p4 = getPoint(X, Y+1);
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
    if(std::abs(dx) > std::abs(dy))
    {
        if(dx < 0)
            return isVisible(end, start);
        for(int x = std::ceil(start.x); x <= std::floor(end.x); x++)
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
        for(int y = std::ceil(start.y); y <= std::floor(end.y); y++)
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

std::vector<Vector2> Terrain::straightenPath(const std::vector<Vector2>& path) const
{
    std::vector<Vector2> result;
    for(int i = 0; i < path.size()-1; i++)
    {
        int j = i;
        while(j < path.size()-1 && isVisible(path[i], path[j+1]))
            j++;
        result.push_back(path[i]);
        result.push_back(path[j]);
        i = j;
    }
    return result;
}

real Terrain::getHeight() const
{
    return height;
}

real Terrain::getWidth() const
{
    return width;
}
