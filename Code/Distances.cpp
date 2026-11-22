#include "Distances.h"
#include <algorithm>
#include <cmath>
#include <limits>

double distance(const Point&a, const Point&b, DistanceType type) {
  // Straight  : sqrt(dx^2 + dy^2)
  // Manhattan : |dx| + |dy|
  double dx = a.x-b.x;
  double dy = a.y-b.y;
  if (type == DistanceType::Manhattan) {
    return std::abs(dx) + std::abs(dy);
  }
  return std::sqrt(dx*dx + dy*dy);
}

// Brute force - Naive
// every point checks every other so each pair is worked out twice (n x (n-1) distances)
// Every row does the same amount of work so the choice of shcedule makes little difference here
void findNeighbours(const std::vector<Point>&points, DistanceType type, bool parallel, std::vector<double>&nearest, std::vector<double>&furthest) {
  int n = static_cast<int>(points.size()); // size() gives unsigned number, so converting to int allows OpenMP to work
  nearest.resize(n);
  furthest.resize(n);

  // Each i only writes nearest[i] and furthest [i], so both threads dont write to the same place.
  // Meaning its safe to run in parallel wihtout locking anything.
  // schedule(runtime) - OMP_SCHEDULE picks schedule when program runs so different ones can be compared without recompiling.
  #pragma omp parallel for schedule(runtime) if(parallel)
  for (int i=0; i<n; ++i) {
    // start nearest at the largest possible double allowing the first real distanc to always be smaller.
    double minDistance = std::numeric_limits<double>::max();
    double maxDistance = 0.0;
    for (int j=0; j<n; ++j) {
      if (j!=i) { // itself would always be 0 and thereby nearest
        double d = distance(points[i], points[j], type);
        minDistance = std::min(minDistance, d); // keep the smallest
        maxDistance = std::max(maxDistance, d); // keep the largest
      }
    }
    nearest[i] = minDistance;
    furthest[i] = maxDistance;
  }
}

// Halfing the work search - Explained in distances.h and elsewhere.
// About n x n / 2 distances, half of findNeighbours, so same answers, half the time.
// SO row i only checks the points after it (j > i) and duplicates for both.
void findNeighboursHalf(const std::vector<Point>&points, DistanceType type, bool parallel, std::vector<double>&nearest, std::vector<double>&furthest) {
  int n = static_cast<int>(points.size());
  // nothing found yet
  nearest.assign(n, std::numeric_limits<double>::max());
  furthest.assign(n, 0.0);

  // Start the threads, Everything inside { } runs on every thread.
  // Theres no loop sharin yet, thats the 'omp for' later on
  #pragma omp parallel if(parallel)
  {
    // Row i also updates answers for later points j.
    // Problem being two threads could possibly update the same point at the smae time, leading to one being lost.
    // So each thread has its own copy of the results which are then combined at the end.
    std::vector<double> myNearest(n, std::numeric_limits<double>::max());
    std::vector<double> myFurthest(n, 0.0);

    #pragma omp for schedule(runtime)
    // Share rows out between threads.
    // Row i checks n-1-i points meaning early rows have more work than late ones
    // LOOP SCHEDULING IS CRITICAL HERE.
    for (int i = 0; i < n; ++i) { // only the points after i to save dupes
      // start from what earlier rows already found for i
      double minDistance = myNearest[i];
      double maxDistance = myFurthest[i];
      for (int j = i+1; j<n; ++j) {
        double d = distance(points[i], points[j], type);
        minDistance = std::min(minDistance, d);
        maxDistance = std::max(maxDistance, d);
        // same distance also counts for j
        myNearest[j] = std::min(myNearest[j], d);
        myFurthest[j] = std::max(myFurthest[j], d);
      }
      myNearest[i] = minDistance;
      myFurthest[i] = maxDistance;
    }

    // Combine each threads copy into shared results.
    // critical only lets one thread at a time, so they cant clash.
    #pragma omp critical
    for (int k=0; k<n; ++k) {
      nearest[k] = std::min(nearest[k], myNearest[k]);
      furthest[k] = std::max(furthest[k], myFurthest[k]);
    }
  }
}

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

// average of the velues - with parallel, adding is split across threads
double mean(const std::vector<double>&values, bool parallel) {
  int n = static_cast<int>(values.size());
  double sum = 0.0;
  // Each thread adds into its private copyy of sum and OpenMP adds coppies together at the end.
  // Without it threads would all update sum simultaneously and lose some additions
  #pragma omp parallel for reduction(+:sum) if(parallel)
  for (int i=0; i<n; ++i) {
    sum += values[i];
  }
  return sum/n;
}
