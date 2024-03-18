#pragma once

#define NOMINMAX
#include "Model3d.h"
#include "Math.h"

class Ray;
class Scene;
class TerrainMesh;

class Terrain
{
public:
   enum DrawMode
   {
      Normal = 0,
      Wireframe = 1,
      Flat = 2
   };

   std::pair<int, int> getClosestAdmissible(Vector2 v) const; // TODO: temp public
   const Vector3& getPoint(int x, int y) const; // same
   bool isAdmissible(int x, int y) const;

private:
	Model3d terrainModel;
	Scene* scene;
	DrawMode drawMode;
	std::string fileName;
	int pickedTriangle;
	int width, height;

	bool* admissiblePoints;

	std::vector<Vector3> points;
	std::vector<int> triangleIndices;

	void calcAdmissiblePoints();
	bool isTriangleAdmissible(const Vector3& p1, const Vector3& p2, const Vector3& p3) const;
	bool isTriangleAdmissible(int x1, int y1, int x2, int y2, int x3, int y3) const;
	
	bool inBounds(int x, int y) const;

	TerrainMesh* terrainMesh;

	TerrainMesh* createFlatMesh(std::string fileName);
	TerrainMesh* createMesh(std::string fileName);


	bool isVisible(Vector2 start, Vector2 end) const;
	std::vector<Vector2> straightenPath(const std::vector<Vector2>& path) const;

public:
	std::pair<real, Vector2> intersectRayOcclusion(Vector2 pos, Vector2 dir) const;
    std::pair<real, Vector2> intersectCirclePathOcclusion(Vector2 pos, Vector2 pos2, real radius) const;
	void setUp();
	void tearDown();
	Terrain(const std::string& filestr, Scene* scene);
	void draw();
	void setDrawMode(DrawMode d);
	Vector3 intersect(const Ray& ray);
	DrawMode getDrawMode() const;
	std::pair<Vector3, Vector3> getBoundingBox() const;
	std::vector<Vector2> findPath(Vector2 start, Vector2 destination);
	real getElevation(real x, real y) const;
	real getWidth() const;
	real getHeight() const;
};

const real cosMaxSlope = std::cos(35*pi/180);

std::pair<real, Vector2> intersectCircleTrianglePath(Vector2 pos, real radius, Vector2 dir, Vector2 p1, Vector2 p2, Vector2 p3);
