#include <vector>
#include "Model3d.h"

std::pair<std::vector<Vertex3>, std::vector<int>> buildCylinder(real length, real radius, int N);
Model3d* createCylinderModel(real length, real radius, int N);
