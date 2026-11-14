// Nearest and furthest neighbours

#include <cstdlib>
#include <iostream>

#include "Distances.h"
#include "Locations.h"

using std::cout, std::endl;

int main(int argc, char* argv[]) {
  // TEMP --- Read points fom the file that are named
  // IF NOT Making 5 random points and outputting --- TEMP
  std::vector<Point> points;
  if (argc >1) {
    points = readLocations(argv[1]);
  }
  else {
    points = generateRandomLocations(5);
  }

  cout << "Number of points:" <<points.size() <<endl;
  cout << "First point:" <<points[0].x << "," <<points[0].y <<endl;

  
  // TEMP -- test of distance function (0.5,0.7 expected??)
  Point a{0.0, 0.0};
  Point b{0.3, 0.4};
  cout << distance(a, b, DistanceType::Standard) << " "
       << distance(a, b, DistanceType::Manhattan) << endl;

  return EXIT_SUCCESS;
}
