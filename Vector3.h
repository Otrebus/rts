#pragma once
#pragma once

#include <cmath>
#include <sstream>
#include "Utils.h"

class Matrix4;

class Vector3
{
public:
    Vector3(real x, real y, real z);
    ~Vector3();
    Vector3();

    Vector3& operator=(const Vector3&);
    Vector3 operator+(const Vector3&) const;
    Vector3 operator-(const Vector3&) const;
    Vector3 operator-() const;

    Vector3 operator+=(const Vector3& v);
    Vector3 operator-=(const Vector3& v);

    Vector3 operator*=(const Matrix4& m);
    bool operator!() const;
    bool operator!=(const Vector3&) const;
    bool operator==(const Vector3&) const;
    real operator*(const Vector3&) const;
    Vector3 operator^(const Vector3&)	const;
    Vector3 operator/(real) const;
    Vector3 operator/=(real);
    Vector3 operator*=(real);
    Vector3 operator*(real) const;
    real& operator[](int);
    real operator[](int) const;
    
    real Length() const;
    real Length2() const;
    void Normalize();
    Vector3 Normalized() const;
    bool IsValid() const;

    real x, y, z;
};


Vector3::Vector3(real x, real y, real z) : x(x), y(y), z(z)
{
}


Vector3::Vector3()
{
}


Vector3::~Vector3()
{
}


Vector3& Vector3::operator=(const Vector3& v)
{
    x = v.x;
    y = v.y;
    z = v.z;
    return *this;
}


Vector3 Vector3::operator+=(const Vector3& v)
{
    x += v.x;
    y += v.y;
    z += v.z;
    return Vector3(x, y, z);
}


Vector3 Vector3::operator-=(const Vector3& v)
{
    x += v.x;
    y += v.y;
    z += v.z;
    return Vector3(x, y, z);
}


Vector3 Vector3::operator/(real f) const
{
    return Vector3(x/f, y/f, z/f);
}


Vector3 Vector3::operator+(const Vector3& v) const
{
    return Vector3(x+v.x, y+v.y, z+v.z);
}


Vector3 Vector3::operator-(const Vector3& v) const
{
    return Vector3(x-v.x, y-v.y, z-v.z); 
}


real Vector3::operator*(const Vector3& v) const
{
    return x*v.x + y*v.y + z*v.z;
}


Vector3 Vector3::operator^(const Vector3& v) const
{
    return Vector3(y*v.z-z*v.y, z*v.x-x*v.z, x*v.y-y*v.x);
}


real Vector3::Length() const
{
    return sqrt(x*x + y*y + z*z);
}


void Vector3::Normalize()
{
    real l = Length();

    x /= l;
    y /= l;
    z /= l;
}


Vector3 Vector3::Normalized() const
{
    real l = Length();
    return { x/l, y/l, z/l };
}


Vector3 Vector3::operator*(real t) const
{
    return Vector3(t*x, t*y, t*z);
}


Vector3 Vector3::operator/=(real t)
{
    x/= t;
    y/= t;
    z/= t;
    return *this;
}


Vector3 Vector3::operator*=(real t)
{
    x*= t;
    y*= t;
    z*= t;
    return *this;
}


Vector3 operator*(real t, const Vector3& v)
{
    return Vector3(t*v.x, t*v.y, t*v.z);
}


Vector3 Vector3::operator-() const
{
    return Vector3(-x, -y, -z);
}


bool Vector3::operator!() const
{
    return x == 0 && y == 0 && z == 0;
}


real& Vector3::operator[](int t)
{
    return (&x)[t];
}


real Vector3::operator[](int t) const
{
    return (&x)[t];
}


std::ostream& operator<<(std::ostream& s, const Vector3& v)
{
    return(s << "(" << v.x << "," << v.y << "," << v.z << ")");
}


bool Vector3::IsValid() const
{
    return std::isfinite(x) && std::isfinite(y) && std::isfinite(z);
}


bool Vector3::operator==(const Vector3& v) const
{
    return v.x == x && v.y == y && v.z == z;
}


bool Vector3::operator!=(const Vector3& v) const
{
    return v.x != x || v.y != y || v.z != z;
}


real Vector3::Length2() const
{
    return x*x + y*y + z*z;
}


Vector3 Vector3::operator*=(const Matrix4& m)
{
    *this = m**this;
    return *this;
}
