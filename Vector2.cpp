#include "Vector2.h"
#include "Vector3.h"
#include <cmath>
#include <cassert>


Vector2::Vector2(float x, float y) : x(x), y(y)
{
    //assert(!std::isnan(x) && !std::isnan(y));
}

Vector2::Vector2()
{
}

Vector2::~Vector2()
{
}

Vector2& Vector2::operator=(const Vector2& v)
{
    x = v.x;
    y = v.y;
    return *this;
}

Vector2& Vector2::operator+=(const Vector2& v)
{
    x += v.x, y += v.y;
    return *this;
}

Vector2& Vector2::operator-=(const Vector2& v)
{
    x -= v.x, y -= v.y;
    return *this;
}

Vector2 Vector2::operator/(float f) const
{
    assert(f);
    return Vector2(x / f, y / f);
}

Vector2 Vector2::operator+(const Vector2& v) const
{
    return Vector2(x + v.x, y + v.y);
}

Vector2 Vector2::operator-(const Vector2& v) const
{
    return Vector2(x - v.x, y - v.y);
}

float Vector2::operator*(const Vector2& v) const
{
    return x * v.x + y * v.y;
}

float Vector2::length() const
{
    return sqrt(x * x + y * y);
}

void Vector2::normalize()
{
    float l = length();
    assert(l);
    x /= l;
    y /= l;
}

Vector2 Vector2::normalized() const
{
    float l = length();
    assert(l);
    return Vector2(x / l, y / l);
}

Vector2 Vector2::operator*(float t) const
{
    return Vector2(t * x, t * y);
}

Vector2& Vector2::operator/=(float t)
{
    x /= t, y /= t;
    return *this;
}

Vector2& Vector2::operator*=(float t)
{
    x *= t, y *= t;
    return *this;
}

Vector2 operator*(float t, const Vector2& v)
{
    return Vector2(t * v.x, t * v.y);
}

Vector2 Vector2::operator-() const
{
    return Vector2(-x, -y);
}

bool Vector2::operator!() const
{
    return x == 0 && y == 0;
}

float& Vector2::operator[](int t)
{
    return (&x)[t];
}


bool Vector2::isValid() const
{
    return std::isfinite(x) && std::isfinite(y);
}


bool Vector2::operator==(const Vector2& v) const
{
    return v.x == x && v.y == y;
}


bool Vector2::operator!=(const Vector2& v) const
{
    return v.x != x || v.y != y;
}


real Vector2::length2() const
{
    return x*x + y*y;
}


Vector2 Vector2::perp() const
{
    return { -y, x };
}


Vector2 Vector2::rotated(real angle) const
{
    return Vector2(std::cos(angle)*x-sin(angle)*y, std::sin(angle)*x + cos(angle)*y);
}

std::ostream& operator<<(std::ostream& s, const Vector2& v)
{
    return(s << "(" << v.x << "," << v.y << ")");
}

Vector3 Vector2::to3() const
{
    return { x, y, 0 };
}

Vector2 operator*(float t, Vector2 v)
{
    return { t*v.x, t*v.y };
}

real Vector2::operator%(const Vector2& v) const
{
    return x*v.y - v.x*y;
}

Vector2::operator bool() const
{
    return x || y;
}