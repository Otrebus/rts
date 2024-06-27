#pragma once

#define NOMINMAX
#include "Math.h"
#include "Model3d.h"
#include "Utils.h"
#include "Vector2.h"
#include <deque>

class Ray;
class Scene;
class TerrainMesh;

class Terrain
{
public:
    enum DrawMode
    {
        Normal = 0,
        Grid = 1,
        Wireframe = 2,
        Flat = 3
    };

    std::pair<int, int> getClosestAdmissible(Vector2 v) const; // TODO: temp public
    const Vector3& getPoint(int x, int y) const; // same
    bool isAdmissible(int x, int y) const;

private:
    Model3d* terrainModel;
    Scene* scene;
    DrawMode drawMode;
    std::string fileName;
    int pickedTriangle;
    int width, height;

    real* max_R;
    real* min_R;

    real* max_L;
    real* min_L;

    bool* admissiblePoints;

    std::vector<Vector3> points;
    std::vector<int> triangleIndices;

    Terrain();

    void calcAdmissiblePoints();
    bool isTriangleAdmissible(const Vector3& p1, const Vector3& p2, const Vector3& p3) const;
    bool isTriangleAdmissible(int x1, int y1, int x2, int y2, int x3, int y3) const;

    TerrainMesh* terrainMesh;

    TerrainMesh* createFlatMesh(std::string fileName);
    TerrainMesh* createMesh(std::string fileName);
    TerrainMesh* createTexturedMesh(std::string fileName);

    bool isVisible(Vector2 start, Vector2 end) const;

public:
    bool isTriangleAdmissible(Vector2 p) const;
    template <typename Iterator> std::deque<Vector2> straightenPath(Iterator begin, Iterator end) const
    {
        std::deque<Vector2> result = { *begin };
        for(auto it = begin, jt = it+1; it != end; it = jt)
        {
            jt = it+1;
            for(auto kt = jt; kt != end; kt++)
                if(isVisible(result.back(), *kt))
                    jt = kt;
            if(jt != end)
                result.push_back(*jt);
        }
        return result;
    }
    std::pair<real, Vector2> intersectRayOcclusion(Vector2 pos, Vector2 dir) const;
    std::pair<real, Vector2> intersectCirclePathOcclusion(Vector2 pos, Vector2 pos2, real radius) const;
    void init();
    void tearDown();
    Terrain(const std::string& filestr, Scene* scene);
    void draw();
    void setDrawMode(DrawMode d);
    Vector3 intersect(const Ray& ray, real maxT=inf);
    DrawMode getDrawMode() const;
    std::pair<Vector3, Vector3> getBoundingBox() const;
    real getElevation(real x, real y) const;
    Vector3 getNormal(real x, real y) const;

    void calcMinMax();

    int getWidth() const;
    int getHeight() const;
    bool inBounds(int x, int y) const;
};

const real cosMaxSlope = std::cos(35_deg);
