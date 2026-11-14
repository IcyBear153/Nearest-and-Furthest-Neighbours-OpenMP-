// Nearest and furthest neighbours

#include <cstdlib>
#include <iostream>
#include "Locations.h"

using std::cout, std::endl;

int main()
{
  // TEMP --- Making 5 random points and outputting --- TEMP
  std::vector<Point> points = generateRandomLocations(5);
  for (const Point& p : points)
  {
    cout << p.x << ", " << p.y << endl;
  }
  return EXIT_SUCCESS;
}
