#pragma once

#include "Math.h"

class Unit;

std::pair<real, Vector2> intersectCircleTrianglePath(Vector2 pos, real radius, Vector2 dir, Vector2 p1, Vector2 p2, Vector2 p3);
std::pair<real, Vector2> intersectCircleCirclePath(Vector2 p1, real r1, Vector2 p2, real r2, Vector2 dir);

real getArrivalRadius(Vector2 p, const std::vector<Unit*>& units);
real intersectRaySegment(Vector2 pos, Vector2 dir, Vector2 p1, Vector2 p2);