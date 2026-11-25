// Andrew's monotone chain convex hull algorithm 
// Fast search (-a fast) -- kept apart from the main project in ConvexHull
// findNeighboursFast -- skips most pairs by using a grid for the nearest and the 'convex hull' for furthest

#pragma once
#include <vector>
#include "../Distances.h"

// Same answers again without checking every pair:
// a grid for nearest where only nearby cells are checked and the 'convex hull' for furthest.
void findNeighboursFast(const std::vector<Point>&points, DistanceType type, bool parallel, std::vector<double>&nearest, std::vector<double>&furthest);
