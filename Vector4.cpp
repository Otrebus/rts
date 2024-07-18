#include "Matrix4.h"
#include "Vector3.h"
#include "Vector4.h"
#include <iostream>

Vector4::Vector4(real x, real y, real z, real w) : x(x), y(y), z(z), w(w)
{
}

Vector4::Vector4() : x(0), y(0), z(0), w(0)
{
}

Vector4::~Vector4()
{
}

Vector4& Vector4::operator=(const Vector4& v)
{
    x = v.x;
    y = v.y;
    z = v.z;
    w = v.w;
    return *this;
}

Vector4& Vector4::operator+=(const Vector4& v)
{
    x += v.x;
    y += v.y;
    z += v.z;
    w += v.w;
    return *this;
}

Vector4& Vector4::operator-=(const Vector4& v)
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
    w -= v.w;
    return *this;
}

Vector4 Vector4::operator/(real f) const
{
    assert(f);
    return Vector4(x/f, y/f, z/f, w/f);
}

Vector4 Vector4::operator+(const Vector4& v) const
{
    return Vector4(x+v.x, y+v.y, z+v.z, w+v.w);
}

Vector4 Vector4::operator-(const Vector4& v) const
{
    auto ret = Vector4(x-v.x, y-v.y, z-v.z, w-v.w);
    return ret;
}

real Vector4::operator*(const Vector4& v) const
{
    return x*v.x + y*v.y + z*v.z + w*v.w;
}

real Vector4::length() const
{
    return sqrt(x*x + y*y + z*z + w*w);
}

void Vector4::normalize()
{
    real l = length();
    assert(l);
    x /= l;
    y /= l;
    z /= l;
    w /= l;
}

Vector4 Vector4::normalized() const
{
    real l = length();
    assert(l);
    return { x/l, y/l, z/l, w/l };
}

Vector4 Vector4::operator*(real t) const
{
    auto ret = Vector4(t*x, t*y, t*z, t*w);
    return ret;
}

Vector4& Vector4::operator/=(real t)
{
    assert(t);
    x/= t;
    y/= t;
    z/= t;
    w/= t;
    return *this;
}

Vector4& Vector4::operator*=(real t)
{
    x*= t;
    y*= t;
    z*= t;
    w*= t;
    return *this;
}

Vector4 operator*(real t, const Vector4& v)
{
    return Vector4(t*v.x, t*v.y, t*v.z, t*v.w);
}

Vector4 Vector4::operator-() const
{
    return Vector4(-x, -y, -z, -w);
}

bool Vector4::operator!() const
{
    return x == 0 && y == 0 && z == 0 && w == 0;
}

real& Vector4::operator[](int t)
{
    return (&x)[t];
}

real Vector4::operator[](int t) const
{
    return (&x)[t];
}

std::ostream& operator<<(std::ostream& s, const Vector4& v)
{
    return(s << "(" << v.x << "," << v.y << "," << v.z << "," << v.w << ")");
}

bool Vector4::isValid() const
{
    return std::isfinite(x) && std::isfinite(y) && std::isfinite(z) && std::isfinite(w);
}

bool Vector4::operator==(const Vector4& v) const
{
    return v.x == x && v.y == y && v.z == z && v.w == w;
}

bool Vector4::operator!=(const Vector4& v) const
{
    return v.x != x || v.y != y || v.z != z || v.w != w;
}

real Vector4::length2() const
{
    return x*x + y*y + z*z + w*w;
}

Vector4& Vector4::operator*=(const Matrix4& m)
{
    *this = m**this;
    return *this;
}

Vector3 Vector4::to3() const
{
    return { x, y, z };
}

Vector4::operator bool() const
{
    return x || y || z || w;
}

Vector4 operator*(float t, Vector4 v)
{
    return { t*v.x, t*v.y, t*v.z, t*v.w };
}
