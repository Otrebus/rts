#pragma once

#include "Model.h"

class Scene;

class Terrain
{
private:
	Model3d terrainModel;
	Scene* scene;

public:
	Terrain(const std::string& filestr, Scene* scene);
	void Draw();
};
