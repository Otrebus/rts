#pragma once

#include "Utils.h"
#include <cmath>
#include <sstream>

class Matrix4;
class Vector2;

class Vector3
{
public:
    Vector3(real x, real y, real z);
    ~Vector3();
    Vector3();

    Vector3 operator%(const Vector3&) const;
    real operator*(const Vector3&) const;
    Vector3 operator+(const Vector3&) const;
    Vector3 operator-(const Vector3&) const;
    Vector3 operator-() const;

    Vector3& operator=(const Vector3&);
    Vector3& operator+=(const Vector3& v);
    Vector3& operator-=(const Vector3& v);

    Vector3& operator*=(const Matrix4& m);

    explicit operator bool() const;
    bool operator!() const;
    bool operator!=(const Vector3&) const;
    bool operator==(const Vector3&) const;

    Vector3& operator/=(real);
    Vector3& operator*=(real);

    Vector3 operator/(real) const;
    Vector3 operator*(real) const;

    real& operator[](int);
    real operator[](int) const;

    real length() const;
    real length2() const;
    void normalize();
    Vector3 normalized() const;
    bool isValid() const;
    Vector3 perp() const;

    Vector2 to2() const;

    real x, y, z;
};

class ostream;

std::ostream& operator<<(std::ostream& s, const Vector3& v);
Vector3 operator*(float, const Vector3& v);

template<> struct std::hash<Vector3>
{
    std::size_t operator()(const Vector3& v) const
    {
        std::size_t hx = std::hash<float>{}(v.x);
        std::size_t hy = std::hash<float>{}(v.y);
        std::size_t hz = std::hash<float>{}(v.z);

        return hx ^ (hy << 1) ^ (hz << 2);
    }
};
