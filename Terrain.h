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

	Mesh3d CreateFlatMesh(std::string fileName);
	Mesh3d CreateMesh(std::string fileName);

public:
	void SetUp();
	void TearDown();
	Terrain(const std::string& filestr, Scene* scene);
	void Draw();
	void SetDrawMode(DrawMode d);
	DrawMode GetDrawMode() const;
};
