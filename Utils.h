#pragma once

#include <string>
#include <vector>
#include <tuple>

class Matrix4;
class Vector3;

using real = float;

std::tuple<std::vector<Vector3>, int, int> readBMP(std::string filename);

Vector3 rgbToVector(unsigned char r, unsigned char g, unsigned char b);
std::tuple<int, int, int> vectorToRgb(const Vector3& color);