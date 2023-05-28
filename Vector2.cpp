#include "Vector2.h"


Vector2::Vector2(float x, float y) : x(x), y(y)
{
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

Vector2 Vector2::operator+=(const Vector2& v)
{
    x += v.x;
    y += v.y;
    return Vector2(x, y);
}

Vector2 Vector2::operator-=(const Vector2& v)
{
    x -= v.x;
    y -= v.y;
    return Vector2(x, y);
}

Vector2 Vector2::operator/(float f) const
{
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

float Vector2::Length() const
{
    return sqrt(x * x + y * y);
}

void Vector2::Normalize()
{
    float l = Length();
    x /= l;
    y /= l;
}

Vector2 Vector2::Normalized() const
{
    float l = Length();
    return Vector2(x / l, y / l);
}

Vector2 Vector2::operator*(float t) const
{
    return Vector2(t * x, t * y);
}

Vector2 Vector2::operator/=(float t)
{
    x /= t;
    y /= t;
    return *this;
}

Vector2 Vector2::operator*=(float t)
{
    x *= t;
    y *= t;
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

std::ostream& operator<<(std::ostream& s, const Vector2& v)
{
    return(s << "(" << v.x << "," << v.y);
}


bool Vector2::IsValid() const
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


real Vector2::Length2() const
{
    return x*x + y*y;
}
