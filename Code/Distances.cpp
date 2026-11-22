#include "Distances.h"

#include <algorithm>
#include <cmath>
#include <limits>

double distance(const Point& a, const Point& b, DistanceType type) {
  double dx = a.x - b.x;
  double dy = a.y - b.y;
  if (type == DistanceType::Manhattan) {
    return std::abs(dx) + std::abs(dy);
  }
  return std::sqrt(dx * dx + dy * dy);
}

void findNeighbours(const std::vector<Point>& points, DistanceType type, bool parallel,
                    std::vector<double>& nearest, std::vector<double>& furthest) {
  int n = static_cast<int>(points.size());
  nearest.resize(n);
  furthest.resize(n);

  #pragma omp parallel for schedule(runtime) if(parallel)
  for (int i = 0; i < n; ++i) {
    double minDistance = std::numeric_limits<double>::max();
    double maxDistance = 0.0;
    for (int j = 0; j < n; ++j) {
      if (j != i) { // don't measure against itself -- gives 0
        double d = distance(points[i], points[j], type);
        minDistance = std::min(minDistance, d);
        maxDistance = std::max(maxDistance, d);
      }
    }
    nearest[i] = minDistance;
    furthest[i] = maxDistance;
  }
}

void findNeighboursHalf(const std::vector<Point>& points, DistanceType type, bool parallel,
                        std::vector<double>& nearest, std::vector<double>& furthest) {
  int n = static_cast<int>(points.size());
  nearest.assign(n, std::numeric_limits<double>::max());
  furthest.assign(n, 0.0);

  #pragma omp parallel if(parallel)
  {
    std::vector<double> myNearest(n, std::numeric_limits<double>::max());
    std::vector<double> myFurthest(n, 0.0);

    #pragma omp for schedule(runtime)
    for (int i = 0; i < n; ++i) {
      double minDistance = myNearest[i];
      double maxDistance = myFurthest[i];
      for (int j = i + 1; j < n; ++j) {
        double d = distance(points[i], points[j], type);
        minDistance = std::min(minDistance, d);
        maxDistance = std::max(maxDistance, d);
        myNearest[j] = std::min(myNearest[j], d);
        myFurthest[j] = std::max(myFurthest[j], d);
      }
      myNearest[i] = minDistance;
      myFurthest[i] = maxDistance;
    }

    #pragma omp critical
    for (int k = 0; k < n; ++k) {
      nearest[k] = std::min(nearest[k], myNearest[k]);
      furthest[k] = std::max(furthest[k], myFurthest[k]);
    }
  }
}

double mean(const std::vector<double>& values, bool parallel) {
  int n = static_cast<int>(values.size());
  double sum = 0.0;
  #pragma omp parallel for reduction(+:sum) if(parallel)
  for (int i = 0; i < n; ++i) {
    sum += values[i];
  }
  return sum / n;
}
