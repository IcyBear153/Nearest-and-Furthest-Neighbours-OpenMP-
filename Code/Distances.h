// Measuring distance between points and finding each points nearest and furthest neighbour
// Two searches give the same answers:
// findNeighbours  -- brute force (naieve), verbetum to the breif
// findNeighboursHalf -- each pair only only once, aka half the work

#pragma once
#include <vector>
#include "Locations.h"

enum class DistanceType{
  Standard,   // line distance --  sqrt(dx^2+dy^2)
  Manhattan   // grid distance --  |dx|+|dy|
};

// Distance between a and b using the chosen type.
// Theyre passed by reference so no coppies made and the function cant change them.
double distance(const Point&a, const Point&b, DistanceType type);

// For every point, the distance to its nearest and furthest other point:
// nearest[i] and furthest[i] are answers for points[i].
// Theyre passed by reference so no coppies made.
// Brute force checks every pair twice wih i,j and then later with j,i.
void findNeighbours(const std::vector<Point>&points, DistanceType type, bool parallel, std::vector<double>&nearest, std::vector<double>&furthest);

// Same answers as findNeighbours, but each pair is only worked out once and counted for both points.
// since distance i,j is the same as j,i, we save half the work.
void findNeighboursHalf(const std::vector<Point>& points, DistanceType type, bool parallel, std::vector<double>&nearest, std::vector<double>&furthest);

// average of values. with parallel, adding is slit across thereads.
double mean(const std::vector<double>&values, bool parallel);
