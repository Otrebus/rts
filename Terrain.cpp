#include "Terrain.h"
#include "TerrainMaterial.h"
#include "TerrainMesh.h"
#include "Ray.h"
#include "GeometryUtils.h"
#include "TexturedTerrainMaterial.h"


TerrainMesh* Terrain::createMesh(std::string fileName)
{
    auto [colors, width, height] = readBMP(fileName, false);
    std::vector<MeshVertex3d> vertices(width*height);
    points.clear();
    triangleIndices.clear();

    auto H = [&colors, &width, &height] (int x, int y)
    {
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
            vertices[width*y+x].selected = 0;
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
            if(!isTriangleAdmissible(points[a], points[b], points[c]))
                vertices[a].selected = vertices[b].selected = vertices[c].selected = 1;
            if(!isTriangleAdmissible(points[a], points[c], points[d]))
                vertices[a].selected = vertices[c].selected = vertices[d].selected = 1;
        }
    }

    this->width = width;
    this->height = height;

    admissiblePoints = new bool[width*height];
    std::fill(admissiblePoints, admissiblePoints + width*height, true);
    calcAdmissiblePoints();
    
    if(terrainMesh)
        delete terrainMesh;
    Material* mat = new TerrainMaterial();
    return terrainMesh = new TerrainMesh(vertices, triangleIndices, mat);
}

TerrainMesh* Terrain::createTexturedMesh(std::string fileName)
{
    auto [colors, width, height] = readBMP(fileName, false);
    std::vector<MeshVertex3d> vertices(width*height);
    points.clear();
    triangleIndices.clear();

    auto H = [&colors, &width, &height] (int x, int y)
    {
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
            vertices[width*y+x].selected = 0;
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
            if(!isTriangleAdmissible(points[a], points[b], points[c]))
                vertices[a].selected = vertices[b].selected = vertices[c].selected = 1;
            if(!isTriangleAdmissible(points[a], points[c], points[d]))
                vertices[a].selected = vertices[c].selected = vertices[d].selected = 1;
        }
    }

    this->width = width;
    this->height = height;

    admissiblePoints = new bool[width*height];
    std::fill(admissiblePoints, admissiblePoints + width*height, true);
    calcAdmissiblePoints();
    
    if(terrainMesh)
        delete terrainMesh;
    Material* mat = new TexturedTerrainMaterial();
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

    auto H = [&colors, &width, &height] (int x, int y)
    {
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
            vertices[width*y+x].selected = 0;
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
                    vertices2.back().selected = 1;
            }
            for(auto i : { a, c, d } )
            {
                triangleIndices2.push_back(vertices2.size());
                triangleIndices.push_back(i);
                vertices2.push_back(vertices[i]);
                vertices2.back().normal = N2;
                if(!isTriangleAdmissible(points[a], points[c], points[d]))
                    vertices2.back().selected = 1;
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

    auto p = ray.pos, d = ray.dir;

    auto x0 = p.x - d.x*(p.y/d.y);
    auto y0 = 0;
    if(x0 < 0 || x0 > width-2)
    {
        if(x0 < 0)
        {
            x0 = 0;
            y0 = p.y - d.y*(p.x/d.x);
        }
        if(x0 > width-2)
        {
            x0 = width-2;
            y0 = p.y - d.y*(width-2-p.x/d.x);
        }
    }

    real xm = x0, xM, ym, yM;

    xM = int(xm+1);
    xm = int(xm);

    while(true)
    {
        if(xm < 0 || xm >= width-1)
            break;

        if(d.x != 0)
        {
            ym = p.y + d.y*(xm - p.x)/d.x;
            yM = p.y + d.y*(xM - p.x)/d.x;
        }
        else
        {
            ym = 0;
            yM = height;
        }


        int X = int(xm);
        for(int Y = std::max(0.f, std::min(ym, yM)); Y < std::min(height-1, int(std::max(ym, yM)+1)); Y++)
        {
            auto p1 = getPoint(X, Y);
            auto p2 = getPoint(X+1, Y);
            auto p3 = getPoint(X+1, Y+1);
            auto p4 = getPoint(X, Y+1);

            auto [t, u, v] = intersectTriangle(p1, p2, p3, ray);
            if(t > -inf && t < closestT)
                closestT = t;
            else
            {
                auto [t, u, v] = intersectTriangle(p1, p3, p4, ray);
                if(t > -inf && t < closestT)
                    closestT = t;
            }
        }
        if(ray.dir.x*ray.dir.y > 0)
            xm = xM++;
        else
            xM = xm--;
    }

    if(closestT < inf)
        return ray.pos + ray.dir*closestT;

    return { inf, inf, inf };
}


Terrain::Terrain(const std::string& fileName, Scene* scene) : fileName(fileName), scene(scene)
{
    init();
    pickedTriangle = -1;
    scene->setTerrain(this);
};


void Terrain::init()
{
    if(drawMode == Normal)
        createTexturedMesh(fileName);
    else
        createMesh(fileName);
    terrainModel = ModelManager::addModel("terrain", *terrainMesh);
    terrainModel->setScene(scene);
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
        ((TerrainMesh*) (terrainModel->getMeshes()[0]))->setFlat(drawMode == DrawMode::Flat);
    terrainModel->updateUniforms();
    terrainModel->draw();
    if(drawMode != DrawMode::Grid)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

real Terrain::getElevation(real x, real y) const
{
    // TODO: could overflow, check
    // temporary (ha!) hack
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

Vector3 Terrain::getNormal(real x, real y) const
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

    return (v2%v1).normalized();
}

void Terrain::setDrawMode(DrawMode d)
{
    drawMode = d;
    std::cout << "Setting drawmode " << drawMode << std::endl;
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

    if(t < inf)
    {
        auto [u, V] = intersectRayOcclusion(pos+dir*t, dir);
        return { u + t, V };
    }
    return { -inf, { 0, 0 } };
}

bool Terrain::isTriangleAdmissible(Vector2 p) const
{
    int x = p.x, y = p.y;
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

std::deque<Vector2> Terrain::straightenPath(const std::deque<Vector2>& path, int maxSteps) const
{
    std::deque<Vector2> result = { path[0] };
    //for(int i = 0; i < std::min((int)path.size(), maxSteps);)
    for(auto it = path.begin(); it != path.end() && it-path.begin() < maxSteps; )
    {
        auto jt = it;
        for(auto kt = jt+1; kt != path.end(); kt++)
            if(isVisible(result.back(), *kt))
                jt = kt;
        result.push_back(*jt);
        it = std::max(jt, it+1);
    }
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
