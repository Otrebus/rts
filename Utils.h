#pragma once

#include <string>
#include <vector>
#include <tuple>

class Matrix4;
class Vector3;

using real = float;

std::tuple<std::vector<char>, int, int> readBMP(std::string filename);
Matrix4 getCameraMatrix(Vector3 pos, Vector3 lookAt, real fov, real ar);
