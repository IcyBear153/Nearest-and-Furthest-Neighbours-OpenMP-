// Measuring distance between points
// Finding each points nearest and furthest neighbour
// BRUTE FORCE/ native algorithm asked for -- (could be improved on...)

#pragma once
#include "Point.h"

enum class DistanceType{
  Standard,   // line distance --  sqrt(dx^2+dy^2)
  Manhattan   // grid distance --  |dx|+|dy|
};

double distance(const Point&a, const Point&b, DistanceType type);
