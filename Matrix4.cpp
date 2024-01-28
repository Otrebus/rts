#include "Matrix4.h"


Matrix4::Matrix4()
{

}

Matrix4::Matrix4(real m00, real m01, real m02, real m03,
                 real m10, real m11, real m12, real m13,
                 real m20, real m21, real m22, real m23,
                 real m30, real m31, real m32, real m33)
{
    m_val[0][0] = m00; m_val[0][1] = m01; m_val[0][2] = m02; m_val[0][3] = m03;
    m_val[1][0] = m10; m_val[1][1] = m11; m_val[1][2] = m12; m_val[1][3] = m13;
    m_val[2][0] = m20; m_val[2][1] = m21; m_val[2][2] = m22; m_val[2][3] = m23;
    m_val[3][0] = m30; m_val[3][1] = m31; m_val[3][2] = m32; m_val[3][3] = m33;
}


real& Matrix4::operator() (int i, int j)
{
    assert(i >= 0 && j <= 3 && j >= 0 && j <= 3);
    return m_val[i][j];
}


real Matrix4::operator() (int i, int j) const
{
    assert(i >= 0 && j <= 3 && j >= 0 && j <= 3);
    return m_val[i][j];
}


Matrix4::~Matrix4()
{
}


Vector3 Matrix4::operator *(const Vector3& v) const
{
    return Vector3(m_val[0][0]*v.x + m_val[0][1]*v.y + m_val[0][2]*v.z + m_val[0][3],
                   m_val[1][0]*v.x + m_val[1][1]*v.y + m_val[1][2]*v.z + m_val[1][3],
                   m_val[2][0]*v.x + m_val[2][1]*v.y + m_val[2][2]*v.z + m_val[2][3]);
}

Matrix4 Matrix4::operator*(real f) const
{
    return Matrix4(m_val[0][0]*f, m_val[0][1]*f, m_val[0][2]*f, m_val[0][3]*f,
                   m_val[1][0]*f, m_val[1][1]*f, m_val[1][2]*f, m_val[1][3]*f,
                   m_val[2][0]*f, m_val[2][1]*f, m_val[2][2]*f, m_val[2][3]*f,
                   m_val[3][0]*f, m_val[3][1]*f, m_val[3][2]*f, m_val[3][3]*f);
}

Matrix4 Matrix4::operator/(real f) const
{
    return Matrix4(m_val[0][0]/f, m_val[0][1]/f, m_val[0][2]/f, m_val[0][3]/f,
                   m_val[1][0]/f, m_val[1][1]/f, m_val[1][2]/f, m_val[1][3]/f,
                   m_val[2][0]/f, m_val[2][1]/f, m_val[2][2]/f, m_val[2][3]/f,
                   m_val[3][0]/f, m_val[3][1]/f, m_val[3][2]/f, m_val[3][3]/f);
}

Matrix4 Matrix4::operator*(const Matrix4& m) const
{
    Matrix4 result;
    for(int i = 0; i < 4; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            result(i,j) = 0;
            for(int k = 0; k < 4; k++)
                result(i,j) += (*this)(i,k)*m(k,j);
        }
    }
    return result;
}
