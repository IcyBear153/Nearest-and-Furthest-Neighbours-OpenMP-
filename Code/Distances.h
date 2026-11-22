// Measuring distance between points
// Finding each points nearest and furthest neighbour
// BRUTE FORCE/ native algorithm asked for -- (could be improved on...)
#pragma once
#include <vector>
#include "Locations.h"

enum class DistanceType{
  Standard,   // line distance --  sqrt(dx^2+dy^2)
  Manhattan   // grid distance --  |dx|+|dy|
};

double distance(const Point&a, const Point&b, DistanceType type);

void findNeighbours(const std::vector<Point>& points, DistanceType type, bool parallel,
                    std::vector<double>& nearest, std::vector<double>& furthest);

void findNeighboursHalf(const std::vector<Point>& points, DistanceType type, bool parallel,
                        std::vector<double>& nearest, std::vector<double>& furthest);

double mean(const std::vector<double>& values, bool parallel);
