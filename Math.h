#pragma once

#include <tuple>
#include "Utils.h"
#include <cmath>

class Vector3;
class Ray;
class Matrix4;

std::tuple<double, double, double> intersectTriangle(const Vector3& v0, const Vector3& v1, const Vector3& v2, const Ray& ray);

Matrix4 getTranslationMatrix(Vector3 v);
Matrix4 getDirectionMatrix(Vector3 dir, Vector3 up);
Matrix4 getNormalMatrix(const Matrix4& m);

const Vector3& getViewRay(Camera& cam, real mouseX, real mouseY);

extern const Matrix4 identityMatrix;
extern const real pi;
extern const real eps;

real resToScreenX(real x, int xres);
real resToScreenY(real y, int yres);
Vector3 calcNormal(Vector3 a, Vector3 b, Vector3 c);
