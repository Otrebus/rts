#pragma once

#include <string>
#include <vector>
#include <tuple>

class Matrix4;
class Vector3;

using real = float;

std::tuple<std::vector<Vector3>, int, int> readBMP(std::string filename);
Matrix4 getCameraMatrix(Vector3 pos, Vector3 lookAt, real fov, real ar);

Vector3 rgbToVector(int r, int g, int b);
std::tuple<int, int, int> vectorToRgb(const Vector3& color);