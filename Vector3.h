#pragma once
#pragma once

#include <cmath>
#include <sstream>

template <typename T> class Matrix4;

template <typename T> class Vector3
{
public:
    Vector3<T>(T x, T y, T z);
    ~Vector3();
    Vector3();

    Vector3& operator=(const Vector3&);
    Vector3 operator+(const Vector3&) const;
    Vector3 operator-(const Vector3&) const;
    Vector3 operator-() const;

    Vector3 operator+=(const Vector3& v);
    Vector3 operator-=(const Vector3& v);

    Vector3 operator*=(const Matrix4<T>& m);
    bool operator!() const;
    bool operator!=(const Vector3&) const;
    bool operator==(const Vector3&) const;
    T operator*(const Vector3&) const;
    Vector3 operator^(const Vector3&)	const;
    Vector3 operator/(T) const;
    Vector3 operator/=(T);
    Vector3 operator*=(T);
    Vector3 operator*(T) const;
    T& operator[](int);
    T operator[](int) const;
    
    T Length() const;
    T Length2() const;
    void Normalize();
    Vector3 Normalized() const;
    bool IsValid() const;

    T x, y, z;
};


template<typename T> Vector3<T>::Vector3(T x, T y, T z) : x(x), y(y), z(z)
{
}


template<typename T> Vector3<T>::Vector3<T>()
{
}


template <typename T> Vector3<T>::~Vector3()
{
}


template <typename T> Vector3<T>& Vector3<T>::operator=(const Vector3& v)
{
    x = v.x;
    y = v.y;
    z = v.z;
    return *this;
}


template <typename T> Vector3<T> Vector3<T>::operator+=(const Vector3& v)
{
    x += v.x;
    y += v.y;
    z += v.z;
    return Vector3(x, y, z);
}


template <typename T> Vector3<T> Vector3<T>::operator-=(const Vector3& v)
{
    x += v.x;
    y += v.y;
    z += v.z;
    return Vector3(x, y, z);
}


template <typename T> Vector3<T> Vector3<T>::operator/(T f) const
{
    return Vector3(x/f, y/f, z/f);
}


template <typename T> Vector3<T> Vector3<T>::operator+(const Vector3& v) const
{
    return Vector3(x+v.x, y+v.y, z+v.z);
}


template <typename T> Vector3<T> Vector3<T>::operator-(const Vector3& v) const
{
    return Vector3(x-v.x, y-v.y, z-v.z); 
}


template <typename T> T Vector3<T>::operator*(const Vector3& v) const
{
    return x*v.x + y*v.y + z*v.z;
}


template <typename T> Vector3<T> Vector3<T>::operator^(const Vector3& v) const
{
    return Vector3(y*v.z-z*v.y, z*v.x-x*v.z, x*v.y-y*v.x);
}


template <typename T> T Vector3<T>::Length() const
{
    return sqrt(x*x + y*y + z*z);
}


template <typename T> void Vector3<T>::Normalize()
{
    T l = Length();

    x /= l;
    y /= l;
    z /= l;
}


template <typename T> Vector3<T> Vector3<T>::Normalized() const
{
    T l = Length();
    return { x/l, y/l, z/l };
}


template <typename T> Vector3<T> Vector3<T>::operator*(T t) const
{
    return Vector3(t*x, t*y, t*z);
}


template <typename T> Vector3<T> Vector3<T>::operator/=(T t)
{
    x/= t;
    y/= t;
    z/= t;
    return *this;
}


template <typename T> Vector3<T> Vector3<T>::operator*=(T t)
{
    x*= t;
    y*= t;
    z*= t;
    return *this;
}


template <typename T> Vector3<T> operator*(T t, const Vector3<T>& v)
{
    return Vector3(t*v.x, t*v.y, t*v.z);
}


template <typename T> Vector3<T> Vector3<T>::operator-() const
{
    return Vector3(-x, -y, -z);
}


template <typename T> bool Vector3<T>::operator!() const
{
    return x == 0 && y == 0 && z == 0;
}


template <typename T> T& Vector3<T>::operator[](int t)
{
    return (&x)[t];
}


template <typename T> T Vector3<T>::operator[](int t) const
{
    return (&x)[t];
}


template <typename T> std::ostream& operator<<(std::ostream& s, const Vector3<T>& v)
{
    return(s << "(" << v.x << "," << v.y << "," << v.z << ")");
}


template<typename T> bool Vector3<T>::IsValid() const
{
    return std::isfinite(x) && std::isfinite(y) && std::isfinite(z);
}


template <typename T> bool Vector3<T>::operator==(const Vector3<T>& v) const
{
    return v.x == x && v.y == y && v.z == z;
}


template <typename T>bool Vector3<T>::operator!=(const Vector3<T>& v) const
{
    return v.x != x || v.y != y || v.z != z;
}


template <typename T> T Vector3<T>::Length2() const
{
    return x*x + y*y + z*z;
}


template <typename T> Vector3<T> Vector3<T>::operator*=(const Matrix4<T>& m)
{
    *this = m**this;
    return *this;
}
