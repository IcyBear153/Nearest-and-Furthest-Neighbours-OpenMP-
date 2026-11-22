// Locations -- where points come from,
// point type, plus two ways to obitain points being reading from CSV or randomly.

// only inlcude this file once (main.cpp includes it and Distances.h)
#pragma once
#include <string>
#include <vector>

// A position (x, y) in 0 to 1.
struct Point {
  double x;
  double y;
};

// returns count points with x, y chosen uniformly at random from [0, 1)
std::vector<Point> generateRandomLocations(int count);
// reads from csv with one x,y pair per line and stops if file cant be accessed
std::vector<Point> readLocations(const std::string&filename);
