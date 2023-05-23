#pragma once
#pragma once

#include <cmath>
#include <sstream>
#include "Utils.h"

template <typename T> class Matrix4;

template <typename T> class Vector2
{
public:
    Vector2(real x, real y);
    ~Vector2();
    Vector2();

    Vector2& operator=(const Vector2&);
    Vector2 operator+(const Vector2&) const;
    Vector2 operator-(const Vector2&) const;
    Vector2 operator-() const;

    Vector2 operator+=(const Vector2& v);
    Vector2 operator-=(const Vector2& v);

    Vector2 operator*=(const Matrix4<T>& m);
    bool operator!() const;
    bool operator!=(const Vector2&) const;
    bool operator==(const Vector2&) const;
    real operator*(const Vector2&) const;

    Vector2 operator/(real) const;
    Vector2 operator/=(real);
    Vector2 operator*=(real);
    Vector2 operator*(real) const;
    real& operator[](int);
    real operator[](int) const;
    
    real Length() const;
    real Length2() const;
    void Normalize();
    Vector2 Normalized() const;
    bool IsValid() const;

    real x, y;
};


Vector2::Vector2(real x, real y) : x(x), y(y)
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


template <typename T> Vector2<T> Vector2<T>::operator-=(const Vector2& v)
{
    x += v.x;
    y += v.y;
    return Vector2(x, y);
}


template <typename T> Vector2<T> Vector2<T>::operator/(T f) const
{
    return Vector2(x/f, y/f);
}


template <typename T> Vector2<T> Vector2<T>::operator+(const Vector2& v) const
{
    return Vector2(x+v.x, y+v.y);
}


template <typename T> Vector2<T> Vector2<T>::operator-(const Vector2& v) const
{
    return Vector2(x-v.x, y-v.y);
}


template <typename T> T Vector2<T>::operator*(const Vector2& v) const
{
    return x*v.x + y*v.y;
}


template <typename T> T Vector2<T>::Length() const
{
    return sqrt(x*x + y*y);
}


template <typename T> void Vector2<T>::Normalize()
{
    T l = Length();

    x /= l;
    y /= l;
}


template <typename T> Vector2<T> Vector2<T>::Normalized() const
{
    T l = Length();
    return { x/l, y/l };
}


template <typename T> Vector2<T> Vector2<T>::operator*(T t) const
{
    return Vector2(t*x, t*y);
}


template <typename T> Vector2<T> Vector2<T>::operator/=(T t)
{
    x/= t;
    y/= t;
    return *this;
}


template <typename T> Vector2<T> Vector2<T>::operator*=(T t)
{
    x*= t;
    y*= t;
    return *this;
}


template <typename T> Vector2<T> operator*(T t, const Vector2<T>& v)
{
    return Vector2(t*v.x, t*v.y);
}


template <typename T> Vector2<T> Vector2<T>::operator-() const
{
    return Vector2(-x, -y);
}


template <typename T> bool Vector2<T>::operator!() const
{
    return x == 0 && y == 0;
}


template <typename T> T& Vector2<T>::operator[](int t)
{
    return (&x)[t];
}


template <typename T> T Vector2<T>::operator[](int t) const
{
    return (&x)[t];
}


template <typename T> std::ostream& operator<<(std::ostream& s, const Vector2<T>& v)
{
    return(s << "(" << v.x << "," << v.y);
}


template<typename T> bool Vector2<T>::IsValid() const
{
    return std::isfinite(x) && std::isfinite(y) && std::isfinite(z);
}


template <typename T> bool Vector2<T>::operator==(const Vector2<T>& v) const
{
    return v.x == x && v.y == y;
}


template <typename T>bool Vector2<T>::operator!=(const Vector2<T>& v) const
{
    return v.x != x || v.y != y;
}


template <typename T> T Vector2<T>::Length2() const
{
    return x*x + y*y;
}


template <typename T> Vector2<T> Vector2<T>::operator*=(const Matrix4<T>& m)
{
    *this = m**this;
    return *this;
}
