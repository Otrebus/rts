#pragma once

#include "Math.h"

class Unit;
class Mesh3d;
class Model3d;
class LineMesh3d;

std::pair<real, Vector2> intersectCircleLinePath(Vector2 pos, real radius, Vector2 dir, Vector2 p1, Vector2 p2);
std::pair<real, Vector2> intersectCircleTrianglePath(Vector2 pos, real radius, Vector2 dir, Vector2 p1, Vector2 p2, Vector2 p3);
std::pair<real, Vector2> intersectCircleCirclePath(Vector2 p1, real r1, Vector2 p2, real r2, Vector2 dir);
std::pair<Vector2, Vector2> intersectCircleCircle(Vector2 c1, real r1, Vector2 c2, real r2);

std::pair<Vector2, Vector2> getCircleTangents(Vector2 c, real R, Vector2 p);

real getArrivalRadius(Vector2 p, const std::vector<Unit*>& units);

real intersectRaySegment(Vector2 pos, Vector2 dir, Vector2 p1, Vector2 p2);
real intersectSegmentSegment(Vector2 r1, Vector2 r2, Vector2 p1, Vector2 p2);

Mesh3d* splitMesh(Mesh3d& mesh, Vector3 pos, Vector3 dir);
Model3d* splitModel(Model3d& model, Vector3 pos, Vector3 dir);

LineMesh3d* splitMeshIntoLineMesh(Mesh3d& mesh, Vector3 pos, Vector3 dir, Vector3 color);
Model3d* splitModelIntoLineModel(Model3d& mesh, Vector3 pos, Vector3 dir, Vector3 color);
Model3d* splitModelAsConstructing(Model3d& sourceModel, real height, Vector3 pos, Vector3 up, real completion);
