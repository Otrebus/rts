#pragma once

#include <string>
#include <vector>
#include <tuple>

class Matrix4;
class Vector3;

using real = float;

std::tuple<std::vector<unsigned char>, int, int> readBMP(std::string filename, bool pad = true);
void writeBMP(std::vector<Vector3> v, int width, int height, std::string filename);

Vector3 rgbToVector(unsigned char r, unsigned char g, unsigned char b);
std::tuple<int, int, int> vectorToRgb(const Vector3& color);