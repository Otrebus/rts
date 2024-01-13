#pragma once

#include "Model.h"

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

	TerrainMesh* createFlatMesh(std::string fileName);
	TerrainMesh* createMesh(std::string fileName);

public:
	void setUp();
	void tearDown();
	Terrain(const std::string& filestr, Scene* scene);
	void draw();
	void setDrawMode(DrawMode d);
	DrawMode getDrawMode() const;
	std::pair<Vector3, Vector3> getBoundingBox() const;
};
