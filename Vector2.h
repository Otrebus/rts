#pragma once

#include "Utils.h"
#include <cmath>
#include <iostream>

class Matrix4;

class Vector2
{
public:
    Vector2(real x, real y);
    ~Vector2();
    Vector2();

    Vector2 operator+(const Vector2&) const;
    Vector2 operator-(const Vector2&) const;
    Vector2 operator-() const;
    real operator*(const Vector2&) const;

    Vector2& operator=(const Vector2&);
    Vector2& operator+=(const Vector2& v);
    Vector2& operator-=(const Vector2& v);

    bool operator!() const;
    bool operator!=(const Vector2&) const;
    bool operator==(const Vector2&) const;
    explicit operator bool() const;

    Vector2& operator*=(const Matrix4& m);

    Vector2& operator/=(real f);
    Vector2& operator*=(real);

    Vector2 operator/(real) const;
    Vector2 operator*(real) const;

    real operator%(const Vector2&) const;

    real& operator[](int);
    real operator[](int) const;

    Vector2 rotated(real angle) const;
    real length() const;
    real length2() const;
    void normalize();
    Vector2 normalized() const;
    Vector2 perp() const;
    bool isValid() const;

    Vector3 to3() const;
    real x, y;
};

std::ostream& operator<<(std::ostream& s, const Vector2& v);
Vector2 operator*(float, Vector2);