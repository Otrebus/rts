#pragma once

#include <cmath>
#include <sstream>
#include "Utils.h"

class Matrix4;
class Vector2;

class Vector4
{
public:
    Vector4(real x, real y, real z, real w);
    ~Vector4();
    Vector4();

    Vector4 operator%(const Vector4&) const;
    real operator*(const Vector4&) const;
    Vector4 operator+(const Vector4&) const;
    Vector4 operator-(const Vector4&) const;
    Vector4 operator-() const;

    Vector4& operator=(const Vector4&);
    Vector4& operator+=(const Vector4& v);
    Vector4& operator-=(const Vector4& v);

    Vector4& operator*=(const Matrix4& m);

    operator bool() const;
    bool operator!() const;
    bool operator!=(const Vector4&) const;
    bool operator==(const Vector4&) const;

    Vector4& operator/=(real);
    Vector4& operator*=(real);

    Vector4 operator/(real) const;
    Vector4 operator*(real) const;

    real& operator[](int);
    real operator[](int) const;
    
    real length() const;
    real length2() const;
    void normalize();
    Vector4 normalized() const;
    bool isValid() const;

    Vector3 to3() const;

    real x, y, z, w;
};

class ostream;

std::ostream& operator<<(std::ostream& s, const Vector4& v);
Vector4 operator*(float, Vector4);