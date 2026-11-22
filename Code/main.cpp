// Nearest and furthest neighbours

// Takes a set of points in a square on each side, from 0 to 1.
// For every point works out how close and far the nearest and furtherest other points are respectivley.
// It does this the simple/native way, by checking every pair.
// Points can be random (-n) or read from a file (-f).

// Usage:
//     nearest_furthest -n <count> -d std|man -s|-p
//     nearest_furthest -f <file>  -d std|man -s|-p

#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include "Distances.h"
#include "Locations.h"

using std::cout, std::cerr, std::endl;

// Stores the users choices from the command line
struct Settings {
  int count =0;                                // -n: number of random points to make
  std::string filename;                        // -f: CSV file to read the points from
  DistanceType type = DistanceType::Standard;  // -d: which distance to use (std or man)
  bool parallel = false;                       // -s (serial) or -p (parallel)
  bool half = false;                           // -a: naive (every pair twice) or half (each pair once)
};

void printUsage() {
  cerr << "Usage:\n"
       << " nearest_furthest -n <count> -d std|man -s|-p\n"
       << " nearest_furthest -f <file>  -d std|man -s|-p\n"
       << " add -a half to check each pair only once\n";
}

// Fills in settings from the command line. Returns false if it isn't valid.
bool readCommandLine(int argc, char*argv[], Settings&settings) {
  bool serialGiven = false;
  bool parallelGiven = false;

  for (int i=1 ; i<argc; ++i) {
    std::string option = argv[i];
    bool hasValue = (i+1 <argc);


    if (option == "-n" && hasValue) {
      settings.count = std::atoi(argv[++i]);
      if (settings.count <2) {
        cerr<< "!! ERROR -- -n needs to be at least 2 points !!" << endl;
        return false;
      }
    }

    else if (option == "-f" && hasValue) {
      settings.filename = argv[++i];
    }

    else if (option == "-d" && hasValue) {
      std::string value = argv[++i];
      if (value == "std") {
        settings.type = DistanceType::Standard;
      }
      else if (value == "man") {
        settings.type = DistanceType::Manhattan;
      }
      else {
        cerr << "!! ERROR!!! -- -d must be followed by std or man !!" << endl;
        return false;
      }
    }
    
    else if (option == "-a" && hasValue) {
      std::string value = argv[++i];
      if (value == "naive") {
        settings.half = false;
      }
      else if (value == "half") {
        settings.half = true;
      }
      else {
        cerr << "!! ERROR -- -a must be followed by naive or half !!" << endl;
        return false;
      }
    }


    else if (option == "-s") {
      serialGiven = true;
    }
    else if (option == "-p") {
      parallelGiven = true;
    }

    else {
      cerr << "!! ERROR-- unknown !!" << option << endl;
      return false;
    }
  }
  
  bool countGiven = (settings.count >0);
  bool fileGiven = !settings.filename.empty();

  if (countGiven == fileGiven) {
    cerr<< "!! ERROR -- give EITHER -n or -f !!" <<endl;
    return false;
  }

  if (serialGiven && parallelGiven) {
    cerr << "!! ERROR -- give EITHER -s or -p !!" <<endl;
    return false;
  }

  settings.parallel = parallelGiven;
  return true;
}


// writes one value per line (default formating)
void writeValues(const std::string& filename, const std::vector<double>&values) {
  std::ofstream file(filename);
  for (double value : values) {
    file <<value<< '\n';
  }
}

int main(int argc, char*argv[]) {
  Settings settings;
  if (!readCommandLine(argc,argv, settings)) {
    printUsage();
    return EXIT_FAILURE;
  }

  std::vector<Point> points;
  if (!settings.filename.empty()) {
    points = readLocations(settings.filename);
  }
  else {
    points = generateRandomLocations(settings.count);
  }
  if (points.size() <2) {
    cerr<< "!! ERROR -- -n needs to be at least 2 points !!" << endl;
    return EXIT_FAILURE;
  }

    // Time calculation
  auto startTime = std::chrono::steady_clock::now();
  
  
  std::vector<double> nearest;
  std::vector<double> furthest;

  if (settings.half) {
    findNeighboursHalf(points, settings.type, settings.parallel, nearest, furthest);
  }
  else {
    findNeighbours(points, settings.type, settings.parallel, nearest, furthest);
  }
  double meanNearest = mean(nearest, settings.parallel);
  double meanFurthest = mean(furthest, settings.parallel);


  //take time on clock from the start time
  double timeTaken = std::chrono::duration<double>(std::chrono::steady_clock::now() -startTime).count(); 

  writeValues("nearest.csv", nearest);
  writeValues("furthest.csv", furthest);

  cout << "Mean distance to nearest neighbour: "  <<meanNearest  <<endl;
  cout << "Mean distance to furthest neighbour: " <<meanFurthest <<endl;
  cout << std::fixed << std::setprecision(3);
  cout << "Time taken: " << timeTaken << " seconds" << endl;

  return EXIT_SUCCESS;
}