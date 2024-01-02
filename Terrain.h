#pragma once

#include "Model.h"

class Scene;

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

	Mesh3d createFlatMesh(std::string fileName);
	Mesh3d createMesh(std::string fileName);

public:
	void setUp();
	void tearDown();
	Terrain(const std::string& filestr, Scene* scene);
	void draw();
	void setDrawMode(DrawMode d);
	DrawMode getDrawMode() const;
};
