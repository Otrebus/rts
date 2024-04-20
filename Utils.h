#pragma once

#include <string>
#include <vector>
#include <tuple>
#include <limits>

class Matrix4;
class Vector3;
class Vector2;
class Entity;

using real = float;

const real inf = std::numeric_limits<real>::infinity();

std::tuple<std::vector<unsigned char>, int, int> readBMP(std::string filename, bool pad = true);
void writeBMP(std::vector<Vector3> v, int width, int height, std::string filename);

Vector3 rgbToVector(unsigned char r, unsigned char g, unsigned char b);
std::tuple<int, int, int> vectorToRgb(const Vector3& color);

real getArrivalRadius(Vector2 p, const std::vector<Entity*>& entities); // TODO: stuff like this should be moved to a more specialized place

#pragma once

#define NOMINMAX
#include <string>
#include <vector>

class Vector3d;

std::vector<std::string> split(const std::string& s, char delim);
std::string lower(const std::string& s);

Vector2 mouseCoordToScreenCoord(int xres, int yres, int mouseX, int mouseY);

template<typename T> T sgn(T s)
{
    return s > 0 ? 1 : s < 0 ? -1 : 0;
}

template<typename T> T min(T a, T b)
{
    return std::min(a, b);
}

template <typename T, typename ...U> T min(T a, T b, U... c)
{
    return min(min(a, b), c...);
}

template<typename T> T max(T a, T b)
{
    return std::max(a, b);
}

template <typename T, typename ...U> T max(T a, T b, U... c)
{
    return max(max(a, b), c...);
}

