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
