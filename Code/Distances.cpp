#include "Distances.h"

#include <cmath>
#include <limits>


double distance(const Point& a, const Point& b, DistanceType type) {
  double dx = (a.x) - (b.x);
  double dy = (a.y)- (b.y);

  if (type == DistanceType::Manhattan) {
    return std::abs(dx) + std::abs(dy);
  }
  return std::sqrt(dx*dx + dy*dy);
}


// Compares i with every point and stores smalest and largest
// distances in nearest[i] and furthest[i] -- only used inside THIS file
static void processPoint(int i, const std::vector<Point>& points, DistanceType type,
                         std::vector<double>& nearest, std::vector<double>& furthest) {

  int n = static_cast<int>(points.size());
  double minDistance = std::numeric_limits<double>::max();
  double maxDistance = 0.0;

  for (int j=0; j<n; ++j) {
    if (j == i) {
      continue; // dont measure against itself -- gives 0
    }
    double d = distance(points[i], points[j], type);
    if (d < minDistance) {
      minDistance = d;
    }
    if (d > maxDistance) {
      maxDistance = d;
    }
  }

  nearest[i] = minDistance;
  furthest[i] = maxDistance;
}

void findNeighboursSerial(const std::vector<Point>& points, DistanceType type,
                          std::vector<double>& nearest, std::vector<double>& furthest) {

  int n = static_cast<int>(points.size());
  nearest.resize(n);
  furthest.resize(n);

  for (int i=0; i<n; ++i) {
    processPoint(i, points, type, nearest, furthest);
  }
}

double meanSerial(const std::vector<double>& values) {
  int n = static_cast<int>(values.size());
  double sum = 0.0;
  for (int i=0; i<n; ++i) {
    sum += values[i];
  }
  return sum / n;
}
