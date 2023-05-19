#pragma once

#include <string>
#include <vector>
#include <tuple>
#include "Vector3.h"
#include "Matrix4.h"

std::tuple<std::vector<char>, int, int> readBMP(std::string filename);
Matrix4<float> getCameraMatrix(Vector3<float> pos, Vector3<float> lookAt, float fov, float ar);