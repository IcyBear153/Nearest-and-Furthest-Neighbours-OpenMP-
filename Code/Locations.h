// functions for getting the object positions
// either random or from a file
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
// reads from csv with one x,y pair per line
std::vector<Point> readLocations(const std::string&filename);
