// Nearest and furthest neighbours
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "Distances.h"
#include "Locations.h"

using std::cout, std::endl;

// Write values to a file, one per line with default number format and no headr
void writeValues(const std::string& filename, const std::vector<double>& values) {
  std::ofstream file(filename);

  for (double value : values) {
    file << value << '\n';
  }
}


int main(int argc, char* argv[]) {

  // TEMP --- Read points fom the file that are named
  // IF NOT Making 1000 random points and outputting --- TEMP
  // this is to save me time when running. Since the nieve algorithm is so slow, I use my own data

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
 
  double meanNearest = meanSerial(nearest);
  double meanFurthest = meanSerial(furthest);

  writeValues("nearest.csv", nearest);
  writeValues("furthest.csv", furthest);

  cout << "Mean distance to Nearest Neighbour :" << meanNearest  <<endl;
  cout << "Mean distance to Furthest Neighbour:" << meanFurthest <<endl;

  return EXIT_SUCCESS;
}