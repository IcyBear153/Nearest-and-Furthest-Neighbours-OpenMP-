// Nearest and furthest neighbours

#include <cstdlib>
#include <iostream>

#include "Distances.h"
#include "Locations.h"

using std::cout, std::endl;

int main(int argc, char* argv[]) {

  // TEMP --- Read points fom the file that are named
  // IF NOT Making 1000 random points and outputting --- TEMP

  std::vector<Point> points;
  if (argc >1) {
    points = readLocations(argv[1]);
  }
  else {
    points = generateRandomLocations(1000);
  }

  cout << "Number of points:" <<points.size() <<endl;
  cout << "First point:" <<points[0].x << "," <<points[0].y <<endl;

  std::vector<double> nearest;
  std::vector<double> furthest;

  findNeighboursSerial(points, DistanceType::Standard, nearest, furthest);
 
  for (int i=0; i<3; ++i) {
    cout << "Point " <<i<< ": nearest " <<nearest[i]<< ",furthest" <<furthest[i]<< endl;
  }
  
  return EXIT_SUCCESS;
}