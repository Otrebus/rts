#pragma once

#define NOMINMAX
#include "Model3d.h"

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

private:
	Model3d terrainModel;
	Scene* scene;
	DrawMode drawMode;
	std::string fileName;
	int pickedTriangle;
	int width, height;

	std::vector<Vector3> points;
	std::vector<int> triangleIndices;

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
	void intersect(const Ray& ray);
	DrawMode getDrawMode() const;
	std::pair<Vector3, Vector3> getBoundingBox() const;
};
