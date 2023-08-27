#pragma once

#include "Model.h"

class Terrain
{
	Model3d terrainModel;

	void readFromFile(const std::string& filestr);
};
