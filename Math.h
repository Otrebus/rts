#pragma once

#include "Utils.h"
#include <cmath>

class Vector3;
class Ray;
class Matrix4;

extern const Matrix4 identityMatrix;
extern const real pi;
extern const real eps;
extern const real gravity;

std::tuple<real, real, real> intersectTriangle(const Vector3& v0, const Vector3& v1, const Vector3& v2, const Ray& ray);

Matrix4 getTranslationMatrix(const Vector3& v);
Matrix4 getDirectionMatrix(const Vector3& dir, const Vector3& up);
Matrix4 getNormalMatrix(const Matrix4& m);
Matrix4 getScalingMatrix(const Vector3& v);

Vector2 resToScreen(real x, real y, int xres, int yres);
Vector3 calcNormal(Vector3 a, Vector3 b, Vector3 c);
Vector3 rebaseOrtho(const Vector3& v, const Vector3& a, const Vector3& b, const Vector3& c);

constexpr real operator"" _deg(long double deg)
{
    return real(pi*deg/180);
}

constexpr real operator"" _deg(unsigned long long int deg)
{
    return real(pi * static_cast<real>(deg) / 180);
}

real deg(real rad);
