#pragma once

#include "Vector3.h"
#include <assert.h>

template<typename T> class Matrix4
{
public:
    Matrix4();
    Matrix4(T m00, T m01, T m02, T m03,
            T m10, T m11, T m12, T m13,
            T m20, T m21, T m22, T m23,
            T m30, T m31, T m32, T m33);
    ~Matrix4();

    Vector3<T> operator *(const Vector3<T>&) const;
    Matrix4 operator *(const Matrix4& v) const;
    T& operator() (int i, int j);
    T operator() (int i, int j) const;

    T m_val[4][4];
};


template <typename T> Matrix4<T>::Matrix4()
{

}


template <typename T> Matrix4<T>::Matrix4(
    T m00, T m01, T m02, T m03,
    T m10, T m11, T m12, T m13,
    T m20, T m21, T m22, T m23,
    T m30, T m31, T m32, T m33)
{
    m_val[0][0] = m00; m_val[0][1] = m01; m_val[0][2] = m02; m_val[0][3] = m03;
    m_val[1][0] = m10; m_val[1][1] = m11; m_val[1][2] = m12; m_val[1][3] = m13;
    m_val[2][0] = m20; m_val[2][1] = m21; m_val[2][2] = m22; m_val[2][3] = m23;
    m_val[3][0] = m30; m_val[3][1] = m31; m_val[3][2] = m32; m_val[3][3] = m33;
}


template <typename T> T& Matrix4<T>::operator() (int i, int j)
{
    assert(i >= 0 && j <= 3 && j >= 0 && j <= 3);
    return m_val[i][j];
}


template <typename T> T Matrix4<T>::operator() (int i, int j) const
{
    assert(i >= 0 && j <= 3 && j >= 0 && j <= 3);
    return m_val[i][j];
}


template <typename T> Matrix4<T>::~Matrix4()
{
}


template<typename T> Vector3<T> Matrix4<T>::operator *(const Vector3<T>& v) const
{
    return Vector3d(m_val[0][0]*v.x + m_val[0][1]*v.y + m_val[0][2]*v.z + m_val[0][3],
                    m_val[1][0]*v.x + m_val[1][1]*v.y + m_val[1][2]*v.z + m_val[1][3],
                    m_val[2][0]*v.x + m_val[2][1]*v.y + m_val[2][2]*v.z + m_val[2][3]);
}


template<typename T> Matrix4<T> Matrix4<T>::operator*(const Matrix4& m) const
{
    Matrix4 result;
    for(int i = 0; i < 4; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            result(i, j) = 0;
            for(int k = 0; k < 4; k++)
                result(i, j) += (*this)(i, k)*m(k, j);
        }
    }
    return result;
}
