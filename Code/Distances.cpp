#include "Distances.h"
#include <cmath>

double distance(const Point& a, const Point& b, DistanceType type) {
  double dx = (a.x) - (b.x);
  double dy = (a.y)- (b.y);

  if (type == DistanceType::Manhattan) {
    return std::abs(dx) + std::abs(dy);
  }
  return std::sqrt(dx*dx + dy*dy);
}
