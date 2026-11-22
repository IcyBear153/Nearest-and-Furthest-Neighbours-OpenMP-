#include "Locations.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <random>


std::vector<Point> generateRandomLocations(int count) {
  std::random_device seed;
  std::mt19937 generator(seed());
  std::uniform_real_distribution<double> uniform(0.0,1.0);
  std::vector<Point> points(count);

  for (int i=0; i< count; ++i) {
    points[i].x = uniform(generator);
    points[i].y = uniform(generator);
  }
  return points;
}

std::vector<Point> readLocations(const std::string&filename)
{
  std::ifstream file(filename);
  if (!file) {
    std::cerr<< "ERROR -- can't open - " <<filename<<std::endl;
    std::exit(EXIT_FAILURE);
  }

  std::vector<Point> points;
  Point p;
  char comma;
  while (file >> p.x >> comma >> p.y && comma == ',') {
    points.push_back(p);
  }

  if (!file.eof()) {
    std::cerr<< "ERROR -- can't read - " <<filename<< " after " <<points.size()<< " points" <<std::endl;
    std::exit(EXIT_FAILURE);
  }
  return points;
}
