#include "Locations.h"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>




std::vector<Point> generateRandomLocations(int count) {
  std::random_device seed;
  std::mt19937 generator(seed());
  std::uniform_real_distribution<double> uniform(0.0, 1.0);

  std::vector<Point> points(count);
  for (int i = 0; i < count; ++i) {
    points[i].x = uniform(generator);
    points[i].y = uniform(generator);
  }
  return points;
}




std::vector<Point> readLocations(const std::string&filename)
{
  std::ifstream file(filename);
  if (!file) {
    std::cerr << "ERROR -- can't open - " <<filename<< std::endl;
    std::exit(EXIT_FAILURE);
  }

  std::vector<Point> points;
  std::string line;
  while (std::getline(file, line)) {
    // skip blank lines
    if (line.empty() || line == "\r") { // ['\r' for LINUX and MAC users because they use this instead ('learn.microsoft.com')]
      continue;
    }

    std::stringstream stream(line);
    Point p;
    char comma;
    if (!(stream>>p.x>>comma>>p.y) || comma != ',') {
      std::cerr <<"ERROR -- can't read line - \"" <<line<< "\" in " << filename
                << std::endl;
      std::exit(EXIT_FAILURE);
    }
    points.push_back(p);
  }
  return points;
}
