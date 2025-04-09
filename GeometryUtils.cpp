#include "GeometryUtils.h"
#include "Ray.h"
#include "Unit.h"
#include "Mesh3d.h"
#include <cassert>
#include "Vertex3d.h"
#include "Model3d.h"
#include "LineMesh3d.h"
#include "LineModelMaterial.h"
#include "LambertianMaterial.h"

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

real intersectSegmentSegment(Vector2 r1, Vector2 r2, Vector2 p1, Vector2 p2)
{
    auto e = p2 - p1;
    auto c = r1 - p1;
    auto f = r2-r1;
    auto det = (e.x*f.y - f.x*e.y);

    auto s = (c.x*e.y - e.x*c.y)/det;
    auto t = (c.x*f.y - f.x*c.y)/det;
    if(t >= 0 && t <= 1 && s >= 0 && s <= 1)
        return s;
    return -inf;
}

std::tuple<real, Vector2, real> distPointLine(Vector2 p, Vector2 p1, Vector2 p2)
{
    auto e = (p2-p1).normalized();
    auto s = (p-p1)*e;
    return { (p - (p1 + e*s)).length(), p1 + e*s, s };
}


std::pair<real, Vector2> intersectCircleLinePath(Vector2 pos, real radius, Vector2 dir, Vector2 p1, Vector2 p2)
{
    return { inf, { inf, inf } }; // TODO: implement
}

std::pair<Vector2, Vector2> intersectCircleCircle(Vector2 c1, real r1, Vector2 c2, real r2)
{
    real d = (c2-c1).length();
    Vector2 Y = (c2 - c1).perp()/d;
    Vector2 X = (c2 - c1)/d;

    real x = (d*d + r1*r1 - r2*r2)/(2*d);
    auto D = r1*r1 - x*x;
    if(D < 0)
        return { Vector2(inf, inf), Vector2(inf, inf) };
    real y = std::sqrt(D);
    return { c1 + x*X + y*Y, c1 + x*X - y*Y };
}

std::pair<Vector2, Vector2> getTangents(Vector2 c, real R, Vector2 p)
{
    auto v = (c - p);
    auto d = v.length();

    auto L = std::sqrt(d*d - R*R);

    return intersectCircleCircle(p, L, c, R);
}


std::pair<real, Vector2> intersectCircleCirclePath(Vector2 p1, real r1, Vector2 p2, real r2, Vector2 dir)
{
    if((p2-p1).length() < r1+r2)
        return { 0.f, (p1-p2).normalized() };
    auto t = intersectRayCircle(p2, dir, p1, r1+r2);
    return { t, ((p1 + dir*t) - p2).normalized() };
}


std::pair<real, Vector2> intersectCircleTrianglePath(Vector2 pos, real radius, Vector2 dir, Vector2 p1, Vector2 p2, Vector2 p3)
{
    auto pp1 = (p1-p2).perp().normalized(), pp2 = (p2-p3).perp().normalized(), pp3 = (p3-p1).perp().normalized();
    auto e11 = p1 + pp1*radius, e12 = p2 + pp1*radius;
    auto e21 = p2 + pp2*radius, e22 = p3 + pp2*radius;
    auto e31 = p3 + pp3*radius, e32 = p1 + pp3*radius;

    // First check if we're closer than the radius of the circle to the triangle
    if(auto [t, q, s] = distPointLine(pos, p1, p2); t < radius && s > 0 && s < (p2-p1).length())
        return { 0.f, (p1-p2).perp().normalized() };

    if(auto [t, q, s] = distPointLine(pos, p2, p3); t < radius && s > 0 && s < (p3-p2).length())
        return { 0.f, (p2-p3).perp().normalized() };

    if(auto [t, q, s] = distPointLine(pos, p3, p1); t < radius && s > 0 && s < (p1-p3).length())
        return { 0.f, (p3-p1).perp().normalized() };

    if(auto t = (pos - p1).length(); t < radius)
        return { 0.f, (pos - p1).normalized() };

    if(auto t = (pos - p2).length(); t < radius)
        return { 0.f, (pos - p2).normalized() };

    if(auto t = (pos - p3).length(); t < radius)
        return { 0.f, (pos - p3).normalized() };

    real minT = inf;
    Vector2 norm;

    real t = intersectRaySegment(pos, dir, e11, e12);
    if(t >= 0)
        minT = t, norm = pp1;

    t = intersectRaySegment(pos, dir, e21, e22);
    if(t >= 0 && t < minT)
        minT = t, norm = pp2;

    t = intersectRaySegment(pos, dir, e31, e32);
    if(t >= 0 && t < minT)
        minT = t, norm = pp3;

    t = intersectRayCircle(pos, dir, p1, radius);
    // TODO: we probably want different normals here
    if(t >= 0 && t < minT)
    {
        minT = t;
        norm = (pos+dir*t-p1).normalized();
    }

    t = intersectRayCircle(pos, dir, p2, radius);
    if(t >= 0 && t < minT)
    {
        minT = t;
        norm = (pos+dir*t-p2).normalized();
    }

    t = intersectRayCircle(pos, dir, p3, radius);
    if(t >= 0 && t < minT)
    {
        minT = t;
        norm = (pos+dir*t-p3).normalized();
    }

    return { minT, norm };
}

Vector3 rebaseOrtho(const Vector3& v, const Vector3& a, const Vector3& b, const Vector3& c)
{
    assert(std::abs(a.length()-1) < 0.01);
    assert(std::abs(b.length()-1) < 0.01);
    assert(std::abs(c.length()-1) < 0.01);

    return { v*a, v*b, v*c };
}


std::tuple<real, real, real> intersectTriangle(const Vector3& v0, const Vector3& v1, const Vector3& v2, const Ray& ray)
{
    real u, v, t;
    const Vector3& D = ray.dir;

    Vector3 E1 = v1-v0, E2 = v2-v0;
    Vector3 T = ray.pos - v0;

    Vector3 P = E2%T, Q = E1%D;

    real det = E2*Q;
    if(!det)
        return { -inf, -inf, -inf };
    u = ray.dir*P/det;

    if(u > 1 || u < 0)
        return { -inf, -inf, -inf };

    v = T*Q/det;

    if(u+v > 1 || u < 0 || v < 0)
        return { -inf, -inf, -inf };

    t = E1*P/det;

    return { t <= 0 ? -inf : t, u, v };
}


real getArrivalRadius(Vector2 p, const std::vector<Unit*>& units)
{
    real L = 0.5, R = 10.0;
    while(R - L > 0.1)
    {
        auto M = (R+L)/2;
        real sum = 0;
        for(auto& unit : units)
        {
            if(auto w = (unit->geoPos-p).length(); w < M)
                sum += 1.5f*1.5f*pi;
        }
        if(sum < pi*M*M)
            R = M;
        else
            L = M;
    }
    return L;
}


real scalarProj(Vector3 u, Vector3 v)
{
    return u*v;
}


template<> struct std::hash<Vertex3>
{
    std::size_t operator()(const Vertex3& v) const noexcept
    {
        std::size_t h1 = std::hash<Vector3>()(v.pos);
        std::size_t h2 = std::hash<Vector2>()(v.tex);
        std::size_t h3 = std::hash<Vector3>()(v.normal);
        return h1 ^ (h2 << 1) ^ (h3 << 1);
    }
};

Mesh3d* splitMesh(Mesh3d& mesh, Vector3 pos, Vector3 dir)
{
    int num = 0;
    std::unordered_map<Vertex3, int> vMap;
    std::vector<int> outTri;
    std::vector<Vertex3> vOut;

    dir.normalize();
    for(int i = 0; i < mesh.triangles.size(); i += 3)
    {
        std::vector<int> out;
        auto& tris = mesh.triangles;
        int T[3] = { tris[i], tris[i+1], tris[i+2] };
        for(int j = 0; j < 3; j++)
        {
            auto u = mesh.v[T[j]], v = mesh.v[T[(j+1)%3]];
            auto up = scalarProj(dir, (u.pos - pos));
            auto vp = scalarProj(dir, (v.pos - pos));
            if(up >= 0)
            {
                if(auto it = vMap.find(u); it == vMap.end())
                    vMap[u] = num++, vOut.push_back(u);
                out.push_back(vMap[u]);
            }

            if(vp < 0 && up > 0 || vp > 0 && up < 0)
            {
                up = std::abs(up);
                vp = std::abs(vp);
                auto pos = (u.pos*vp + v.pos*up)/(up + vp);
                auto n = ((u.normal*vp + v.normal*up)/(up + vp)).normalized();
                auto tex = (u.tex*vp + v.tex*up)/(up + vp);

                auto vx = Vertex3(pos, n, tex);

                if(auto it = vMap.find(vx); it == vMap.end())
                    vMap[vx] = num++, vOut.push_back(vx);
                out.push_back(vMap[vx]);
            }
        }
        for(int i = 1; i < ((int)out.size()-1); i++)
        {
            outTri.push_back(out[0]);
            outTri.push_back(out[i]);
            outTri.push_back(out[1+i]);
        }
    }

    return new Mesh3d(vOut, outTri, mesh.material);
}

LineMesh3d* splitMeshIntoLineMesh(Mesh3d& mesh, Vector3 pos, Vector3 dir, Vector3 color = Vector3(1.f, 1.f, 1.f))
{
    int num = 0;
    std::unordered_map<Vector3, int> vMap;
    std::vector<std::pair<int, int>> outLines;
    std::vector<Vector3> vOut;

    dir.normalize();
    for(int i = 0; i < mesh.triangles.size(); i += 3)
    {
        std::vector<int> out;
        auto& tris = mesh.triangles;
        int T[3] = { tris[i], tris[i+1], tris[i+2] };
        for(int j = 0; j < 3; j++)
        {
            auto u = mesh.v[T[j]].pos, v = mesh.v[T[(j+1)%3]].pos;
            auto up = scalarProj(dir, (u - pos));
            auto vp = scalarProj(dir, (v - pos));
            if(up >= 0)
            {
                if(auto it = vMap.find(u); it == vMap.end())
                    vMap[u] = num++, vOut.push_back(u);
                out.push_back(vMap[u]);
            }

            if(vp < 0 && up > 0 || vp > 0 && up < 0)
            {
                up = std::abs(up);
                vp = std::abs(vp);
                auto pos = (u*vp + v*up)/(up + vp);

                auto vx = Vector3(pos);

                if(auto it = vMap.find(vx); it == vMap.end())
                    vMap[vx] = num++, vOut.push_back(vx);
                out.push_back(vMap[vx]);
            }
        }
        for(int i = 0; i < (int)out.size(); i++)
        {
            outLines.push_back( { out[i], out[(i+1)%(out.size())] } );
        }
    }

    auto mat = new LineModelMaterial(color);
    return new LineMesh3d(vOut, outLines, mat);
}

Model3d* splitModelIntoLineModel(Model3d& sourceModel, Vector3 pos, Vector3 dir, Vector3 color)
{
    auto model = new Model3d();
    model->pos = sourceModel.pos;
    model->setDirection(sourceModel.dir, sourceModel.up);
    model->setSize(Vector3(1, 1, 1));
    auto modelMatrix = model->getTransformationMatrix();

    for (auto& mesh : sourceModel.meshes)
    {
        auto newMesh = new Mesh3d(mesh->v, mesh->triangles, mesh->material);
        newMesh->transform(modelMatrix);
        model->addMesh(*splitMeshIntoLineMesh(*newMesh, pos, dir, color));
        delete newMesh;
    }

    model->setDirection(Vector3(1, 0, 0), Vector3(0, 0, 1));
    return model;
}


Model3d* splitModel(Model3d& sourceModel, Vector3 pos, Vector3 dir)
{
    auto model = new Model3d();
    model->pos = sourceModel.pos;
    model->setDirection(sourceModel.dir, sourceModel.up);
    model->setSize(Vector3(1, 1, 1));
    auto modelMatrix = model->getTransformationMatrix();

    for (auto& mesh : sourceModel.meshes)
    {
        auto newMesh = new Mesh3d(mesh->v, mesh->triangles, mesh->material);
        newMesh->transform(modelMatrix);
        model->addMesh(*splitMesh(*newMesh, pos, dir));
        delete newMesh;
    }

    model->setDirection(Vector3(1, 0, 0), Vector3(0, 0, 1));
    return model;
}


Model3d* splitModelAsConstructing(Model3d& sourceModel, real height, Vector3 pos, Vector3 up, real completion)
{
    auto z = height;
    auto p = pos - z/2*up;

    auto time = real(glfwGetTime());

    if(completion < 1.f/3.f)
    {
        auto model = splitModelIntoLineModel(sourceModel, p + z*up*(completion*3.0f), -up, Vector3(0.0f, 0.8 + std::cos(time*5)*0.2, 0.f));
        return model;
    }
    else if(completion < 2.f/3.f)
    {
        auto bottom = splitModel(sourceModel, p + z*up*(completion-1.0f/3)*3.0, -up);
        auto top = splitModelIntoLineModel(sourceModel, p + z*up*(completion-1.0f/3.0f)*3.0f, up, Vector3(0.0f, 0.8 + std::cos(time*5)*0.2, 0.f));

        auto model = new Model3d();
        model->pos = sourceModel.pos;
        model->setDirection(sourceModel.dir, sourceModel.up);
        model->setSize(Vector3(1, 1, 1));
        auto modelMatrix = model->getTransformationMatrix();

        auto mat = new LambertianMaterial();
        mat->Kd = Vector3(0.f, 0.8f + std::cos(time*5)*0.2f, 0);
        for(auto& mesh : bottom->meshes)
        {
            auto newMesh = new Mesh3d(mesh->v, mesh->triangles, mat);
            model->addMesh(*newMesh);
        }

        for(auto& mesh : top->meshes)
        {
            auto p = (LineMesh3d*) mesh;
            auto newMesh = new LineMesh3d(p->v, p->lines, p->material);
            model->addMesh(*newMesh);
        }

        model->setDirection(Vector3(1, 0, 0), Vector3(0, 0, 1));
        return model;
    }
    else
    {
        auto top = splitModel(sourceModel, p + z*up*(completion-2.0f/3)*3.0, up);
        auto bottom = splitModel(sourceModel, p + z*up*(completion-2.0f/3)*3.0, -up);

        auto model = new Model3d();
        model->pos = sourceModel.pos;
        model->setDirection(sourceModel.dir, sourceModel.up);
        model->setSize(Vector3(1, 1, 1));
        auto modelMatrix = model->getTransformationMatrix();

        for(auto& mesh : bottom->meshes)
        {
            auto newMesh = new Mesh3d(mesh->v, mesh->triangles, mesh->material);
            model->addMesh(*newMesh);
        }

        auto mat = new LambertianMaterial();
        mat->Kd = Vector3(0.f, 0.8f + std::cos(time*5)*0.2f, 0);
        for(auto& mesh : top->meshes)
        {
            auto newMesh = new Mesh3d(mesh->v, mesh->triangles, mat);
            model->addMesh(*newMesh);
        }

        model->setDirection(Vector3(1, 0, 0), Vector3(0, 0, 1));
        return model;
    }
}