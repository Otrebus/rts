#pragma once

#include <tuple>
#include <cmath>
#include "Utils.h"

class Vector3;
class Ray;
class Matrix4;

extern const Matrix4 identityMatrix;
extern const real pi;
extern const real eps;

std::tuple<double, double, double> intersectTriangle(const Vector3& v0, const Vector3& v1, const Vector3& v2, const Ray& ray);

Matrix4 getTranslationMatrix(Vector3 v);
Matrix4 getDirectionMatrix(Vector3 dir, Vector3 up);
Matrix4 getNormalMatrix(const Matrix4& m);

real resToScreenX(real x, int xres);
real resToScreenY(real y, int yres);
Vector3 calcNormal(Vector3 a, Vector3 b, Vector3 c);
Vector3 rebaseOrtho(const Vector3& v, const Vector3& a, const Vector3& b, const Vector3& c);

constexpr long double operator"" _deg(long double deg)
{
    return pi*deg/180;
}

constexpr long double operator"" _deg(unsigned long long int deg)
{
    return pi * static_cast<long double>(deg) / 180;
}

real deg(real rad);
