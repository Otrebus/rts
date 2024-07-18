#include "Matrix4.h"
#include "Vector2.h"
#include "Vector3.h"


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


Vector3& Vector3::operator+=(const Vector3& v)
{
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
}


Vector3& Vector3::operator-=(const Vector3& v)
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
}


Vector3 Vector3::operator/(real f) const
{
    assert(f);
    return Vector3(x/f, y/f, z/f);
}


Vector3 Vector3::operator+(const Vector3& v) const
{
    return Vector3(x+v.x, y+v.y, z+v.z);
}


Vector3 Vector3::operator-(const Vector3& v) const
{
    return { x-v.x, y-v.y, z-v.z };
}


real Vector3::operator*(const Vector3& v) const
{
    return x*v.x + y*v.y + z*v.z;
}


Vector3 Vector3::operator%(const Vector3& v) const
{
    return { y*v.z-z*v.y, z*v.x-x*v.z, x*v.y-y*v.x };
}


real Vector3::length() const
{
    return sqrt(x*x + y*y + z*z);
}


void Vector3::normalize()
{
    real l = length();
    assert(l);
    x /= l;
    y /= l;
    z /= l;
}


Vector3 Vector3::normalized() const
{
    real l = length();
    assert(l);
    return { x/l, y/l, z/l };
}


Vector3 Vector3::operator*(real t) const
{
    auto ret = Vector3(t*x, t*y, t*z);
    return ret;
}


Vector3& Vector3::operator/=(real t)
{
    assert(t);
    x/= t;
    y/= t;
    z/= t;
    return *this;
}


Vector3& Vector3::operator*=(real t)
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


bool Vector3::isValid() const
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


real Vector3::length2() const
{
    return x*x + y*y + z*z;
}


Vector3& Vector3::operator*=(const Matrix4& m)
{
    *this = m**this;
    return *this;
}

Vector2 Vector3::to2() const
{
    return { x, y };
}

Vector3::operator bool() const
{
    return x || y || z;
}
