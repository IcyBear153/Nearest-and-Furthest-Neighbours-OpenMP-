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

void findNeighboursSerial(const std::vector<Point>& points, DistanceType type,
                          std::vector<double>& nearest, std::vector<double>& furthest);

void findNeighboursParallel(const std::vector<Point>& points, DistanceType type,
                            std::vector<double>& nearest, std::vector<double>& furthest);

// distance(i, j) is the same as distance(j, i), so each pair is now only worked out once and counted for both points
void findNeighboursHalfSerial(const std::vector<Point>& points, DistanceType type,
                              std::vector<double>& nearest, std::vector<double>& furthest);

// The average calculated of list
double meanSerial(const std::vector<double>& values);
double meanParallel(const std::vector<double>& values);
