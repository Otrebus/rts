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
	std::vector<Vector2> findPath(Vector2 start, Vector2 destination);
	
	
	bool inBounds(int x, int y) const;

	TerrainMesh* terrainMesh;

	TerrainMesh* createFlatMesh(std::string fileName);
	TerrainMesh* createMesh(std::string fileName);

	void selectTriangle(int i, bool selected);



public:
	void setUp();
	void tearDown();
	Terrain(const std::string& filestr, Scene* scene);
	void draw();
	void setDrawMode(DrawMode d);
	Vector3 intersect(const Ray& ray);
	DrawMode getDrawMode() const;
	std::pair<Vector3, Vector3> getBoundingBox() const;
	real getElevation(real x, real y) const;
	real getWidth() const;
	real getHeight() const;
};

const real cosMaxSlope = std::cos(35*pi/180);
