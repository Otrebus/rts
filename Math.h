#pragma once

#include <tuple>

class Vector3;
class Ray;

std::tuple<double, double, double> intersectTriangle(const Vector3& v0, const Vector3& v1, const Vector3& v2, const Ray& ray);
Matrix4 getTranslationMatrix(Vector3 v);
Matrix4 getDirectionMatrix(Vector3 dir, Vector3 up);
Matrix4 getNormalMatrix(const Matrix4& m);