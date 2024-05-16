#pragma once

#include "Math.h"

class Unit;

std::pair<real, Vector2> intersectCircleTrianglePath(Vector2 pos, real radius, Vector2 dir, Vector2 p1, Vector2 p2, Vector2 p3);
real getArrivalRadius(Vector2 p, const std::vector<Unit*>& units);
