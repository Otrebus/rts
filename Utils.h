#pragma once
#define NOMINMAX

#include <limits>
#include <string>
#include <vector>

class Matrix4;
class Vector3;
class Vector2;
class Entity;
class Unit;

using real = float;

const real inf = std::numeric_limits<real>::infinity();

std::tuple<std::vector<unsigned char>, int, int> readBMP(std::string filename, bool pad = true);
void writeBMP(std::vector<Vector3> v, int width, int height, std::string filename);

Vector3 rgbToVector(unsigned char r, unsigned char g, unsigned char b);
std::tuple<int, int, int> vectorToRgb(const Vector3& color);
std::string lower(const std::string& str);

class Vector3d;

Vector2 mouseCoordToScreenCoord(int xres, int yres, int mouseX, int mouseY);

template<typename T> T sgn(T s)
{
    return s > T(0) ? T(1) : s < T(0) ? T(-1) : T(0);
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

std::string realToString(real num, int significantDigits);

template<typename T> T flipVertically(T input, int width)
{
    T out;
    int height = int(input.size())/width;
    for(int i = 0; i < input.size(); i++)
    {
        int x = i % width;
        int y = i / width;
        out.push_back(input[(height-1-y)*width + x]);
    }
    return out;
}