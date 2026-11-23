// Andrew's monotone chain convex hull algorithm 
// Fast search (-a fast) -- kept apart from the main project in ConvexHull
#include "FastSearch.h"
#include <algorithm>
#include <cmath>
#include <limits>

// for sorting points left to right or bottom to top (if level)
static bool leftToRight(const Point&a, const Point&b) {
  return a.x<b.x || (a.x==b.x && a.y<b.y);
}
// > 0 if going o to a to b turns left and < 0 if it turns right
static double turn(const Point&o, const Point&a, const Point&b) {
  return (a.x-o.x) * (b.y-o.y) - (a.y-o.y) * (b.x-o.x);
}

// 'Convex hull' - the points a ruber band would touch if you stretched it round all of them (Andrew's monotone chain)
// Goes left to right along the bottom, then right to left along the top, throwing out any point where the band would turn right
static std::vector<Point> convexHull(std::vector<Point> p) {
  std::sort(p.begin(), p.end(), leftToRight);
  std::vector<Point> hull;
  for (int side =0; side <2; ++side) {
    size_t start = hull.size();
    for (const Point& q:p) {
      while (hull.size() >= start +2 && turn(hull[hull.size() -2], hull.back(), q) <0) {
        hull.pop_back();
      }
      hull.push_back(q);
    }
    hull.pop_back(); // the last point starts other side
    std::reverse(p.begin(), p.end());
  }
  return hull;
}


// which of the g grid cells along one side the coordinate v falls in
static int cellOf(double v, int g) {
  return std::min(g-1, static_cast<int>(v*g));
}
// fast search gives the same answers, without checking every pair.
// nearest only look at points in grid cells close to the point.
// furthest point from anywhere is always a corrner of the 'convex hull', so only those few points need checking
void findNeighboursFast(const std::vector<Point>&points, DistanceType type, bool parallel, std::vector<double>&nearest, std::vector<double>&furthest) {
  int n = static_cast<int>(points.size());
  nearest.resize(n);
  furthest.resize(n);

  // gxg grid of cells, about 2 points in each
  int g = std::max(1, static_cast<int>(std::sqrt(n/2.0)));
  double cell = 1.0/g;
  std::vector<std::vector<int>> grid(g*g);
  for (int k=0; k<n; ++k) {
    grid[cellOf(points[k].y, g) * g + cellOf(points[k].x, g)].push_back(k);
  }
  std::vector<Point> hull = convexHull(points);

  #pragma omp parallel for schedule(runtime) if(parallel)
  for (int i=0; i<n; ++i) {
    // look in square of cells round i, one cell 'r' bigger each time
    // Points outside the square are at least 'r' cells away so stop once one clser is found
    int cx = cellOf(points[i].x, g);
    int cy = cellOf(points[i].y, g);
    double best = std::numeric_limits<double>::max();
    for (int r=0; r<g && best > (r-1) * cell; ++r) {
      for (int y = std::max(0, cy-r); y <= std::min(g-1, cy+r); ++y) {
        for (int x = std::max(0, cx-r); x <= std::min(g-1, cx+r); ++x) {
          for (int j : grid[y * g+x]) {
            if (j!=i) {
              best = std::min(best, distance(points[i], points[j], type));
            }
          }
        }
      }
    }
    nearest[i] = best;
    double worst = 0.0;
    for (const Point&h:hull) {
      worst = std::max(worst, distance(points[i], h, type));
    }
    furthest[i] = worst;
  }
}
