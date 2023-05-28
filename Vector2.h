#pragma once

#include <cmath>
#include <iostream>
#include "Utils.h"

class Matrix4;

class Vector2
{
public:
    Vector2(float x, float y);
    ~Vector2();
    Vector2();

    Vector2& operator=(const Vector2&);
    Vector2 operator+(const Vector2&) const;
    Vector2 operator-(const Vector2&) const;
    Vector2 operator-() const;

    Vector2 operator+=(const Vector2& v);
    Vector2 operator-=(const Vector2& v);

    Vector2 operator/=(float f);
    bool operator!() const;
    bool operator!=(const Vector2&) const;
    bool operator==(const Vector2&) const;
    float operator*(const Vector2&) const;

    Vector2 operator/(float) const;
    Vector2 operator*=(float);
    Vector2 operator*=(const Matrix4& m);
    Vector2 operator*(float) const;
    float& operator[](int);
    float operator[](int) const;

    float Length() const;
    float Length2() const;
    void Normalize();
    Vector2 Normalized() const;
    bool IsValid() const;

    float x, y;
};
