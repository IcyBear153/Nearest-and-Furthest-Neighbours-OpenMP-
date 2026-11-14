// functions for getting the object positions
// either random or from a file
#pragma once
#include <vector>
#include "Point.h"

// returns count points with x, y chosen uniformly at random from [0, 1)
std::vector<Point> generateRandomLocations(int count);
