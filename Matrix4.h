#pragma once

#include "Vector3.h"
#include <assert.h>
#include "Utils.h"


class Matrix4
{
public:
    Matrix4();
    Matrix4(real m00, real m01, real m02, real m03,
            real m10, real m11, real m12, real m13,
            real m20, real m21, real m22, real m23,
            real m30, real m31, real m32, real m33);
    ~Matrix4();

    Vector3 operator *(const Vector3&) const;
    Matrix4 operator *(const Matrix4& v) const;
    real& operator() (int i, int j);
    real operator() (int i, int j) const;

    real m_val[4][4];
};
