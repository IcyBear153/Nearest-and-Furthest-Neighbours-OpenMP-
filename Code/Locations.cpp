#include "Locations.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <random>


std::vector<Point> generateRandomLocations(int count) {
  // 'random_device' gives a random number from the OS
  // Is used once to start the generator allowing every run to give different points
  std::random_device seed;
  std::mt19937 generator(seed());  // fast generator that makes long stream of randnumbers from a seed
  std::uniform_real_distribution<double> uniform(0.0,1.0);  // turns the generators numbers into pairs spread over the arg evenly
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
    std::cerr<< "ERROR -- can't open - " <<filename<<std::endl; // wrong name or not in folder, ect
    std::exit(EXIT_FAILURE);
  }

  std::vector<Point> points;
  Point p;
  char comma;
  // skips any spaces and line breaks before a number (\r counts too, so Windows line endings are fine).
  // Keeps going while it reads a number, a comma and a number.
  while (file >> p.x >> comma >> p.y && comma == ',') {
    points.push_back(p);
  }
  // The loop stops at the end of the csv or at an ERROR
  // eof() is only true at end of the file, so anything else is ERROR
  if (!file.eof()) {
    std::cerr<< "ERROR -- can't read - " <<filename<< " after " <<points.size()<< " points" <<std::endl;
    std::exit(EXIT_FAILURE);
  }
  return points;
}
