#include "Locations.h"
#include <random>

std::vector<Point> generateRandomLocations(int count) {
  std::random_device seed;
  std::mt19937 generator(seed());
  std::uniform_real_distribution<double> uniform(0.0, 1.0);

  std::vector<Point> points(count);
  for (int i = 0; i < count; ++i) {
    points[i].x = uniform(generator);
    points[i].y = uniform(generator);
  }
  return points;
}
