// Measuring distance between points
// Finding each points nearest and furthest neighbour
// BRUTE FORCE/ native algorithm asked for -- (could be improved on...)

#pragma once

#include <vector>

#include "Point.h"

enum class DistanceType{
  Standard,   // line distance --  sqrt(dx^2+dy^2)
  Manhattan   // grid distance --  |dx|+|dy|
};

double distance(const Point&a, const Point&b, DistanceType type);


// for every point, find distance to nearest and furthest other point
// (nearest[i] and furthest[i] hold answers for points[i])

void findNeighboursSerial(const std::vector<Point>& points, DistanceType type,
                          std::vector<double>& nearest, std::vector<double>& furthest);