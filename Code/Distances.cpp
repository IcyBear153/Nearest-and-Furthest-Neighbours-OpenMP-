#include "Distances.h"

#include <cmath>
#include <limits>


double distance(const Point&a, const Point&b, DistanceType type) {
  double dx = (a.x) - (b.x);
  double dy = (a.y)- (b.y);

  if (type == DistanceType::Manhattan) {
    return std::abs(dx) + std::abs(dy);
  }
  return std::sqrt(dx*dx + dy*dy);
}


// Compares i with every point and stores smalest and largest
// distances in nearest[i] and furthest[i] -- only used inside THIS file
static void processPoint(int i, const std::vector<Point>&points, DistanceType type,
                         std::vector<double>&nearest, std::vector<double>&furthest) {

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

void findNeighboursSerial(const std::vector<Point>&points, DistanceType type,
                          std::vector<double>&nearest, std::vector<double>&furthest) {

  int n = static_cast<int>(points.size());
  nearest.resize(n);
  furthest.resize(n);

  for (int i=0; i<n; ++i) {
    processPoint(i, points, type, nearest, furthest);
  }
}


void findNeighboursParallel(const std::vector<Point>&points, DistanceType type,
                            std::vector<double>&nearest, std::vector<double>&furthest) {

  int n = static_cast<int>(points.size());
  nearest.resize(n);
  furthest.resize(n);

  // each iteration only writes its nearest[i] and furthest[i]
  // meaning no two threads ever write to the same place so they dont need to wait for eachother (they wont clash its fine to run in paralel)

  // schedule(runtime) is set by OMP_SCHEDULE when the program runs so dont need to recompile to change for different schedules
  #pragma omp parallel for schedule(runtime)
  for (int i=0; i<n; ++i) {
    processPoint(i, points, type, nearest, furthest);
  }
}


// Compares i with every point AFTER it, each distance counts for both points
static void processRow(int i, const std::vector<Point>& points, DistanceType type,
                       std::vector<double>& nearest, std::vector<double>& furthest) {
  int n = static_cast<int>(points.size());

  double minDistance = nearest[i];
  double maxDistance = furthest[i];

  for (int j=i+1; j<n; ++j) {
    double d = distance(points[i], points[j], type);
    if (d < minDistance) {
      minDistance = d;
    }
    if (d > maxDistance) {
      maxDistance = d;
    }
    if (d < nearest[j]) {
      nearest[j] = d;
    }
    if (d > furthest[j]) {
      furthest[j] = d;
    }
  }
  nearest[i] = minDistance;
  furthest[i] = maxDistance;
}

void findNeighboursHalfSerial(const std::vector<Point>&points, DistanceType type,
                              std::vector<double>&nearest, std::vector<double>&furthest) {
  int n = static_cast<int>(points.size());
  // nothing found for any point
  nearest.assign(n, std::numeric_limits<double>::max());
  furthest.assign(n,0.0);

  for (int i=0; i<n; ++i) {
    processRow(i, points, type, nearest, furthest);
  }
}


void findNeighboursHalfParallel(const std::vector<Point>&points, DistanceType type,
                                std::vector<double>&nearest, std::vector<double>&furthest) {

  int n = static_cast<int>(points.size());
  nearest.assign(n, std::numeric_limits<double>::max());
  furthest.assign(n,0.0);

  #pragma omp parallel
  {
    std::vector<double> myNearest(n, std::numeric_limits<double>::max());
    std::vector<double> myFurthest(n, 0.0);

    #pragma omp for schedule(runtime)
    for (int i=0; i<n; ++i) {
      processRow(i, points, type, myNearest, myFurthest);
    }
    #pragma omp critical
    {
      for (int k=0; k<n; ++k) {
        if (myNearest[k] < nearest[k]) {
          nearest[k] = myNearest[k];
        }
        if (myFurthest[k] > furthest[k]) {
          furthest[k] = myFurthest[k];
        }
      }
    }
  }
}


double meanSerial(const std::vector<double>&values) {
  int n = static_cast<int>(values.size());
  double sum = 0.0;
  for (int i=0; i<n; ++i) {
    sum += values[i];
  }
  return sum/n;
}

double meanParallel(const std::vector<double>& values) {
  int n = static_cast<int>(values.size());
  double sum = 0.0;
  // evry thread keeps a running total and openmp combines them into one sum at the end
  #pragma omp parallel for reduction(+:sum)
  for (int i=0; i<n; ++i) {
    sum += values[i];
  }
  return sum/n;
}