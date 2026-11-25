// Nearest and furthest neighbours

// Takes a set of points in the unit square (x,y between 0,1).
// Finds the distance to its nearest and furthest other point for every point.
// Writes these to nearest.csv and furthesst.csv,
// doing so with one per line in the same order it reads the points.
// Then prints the mean of both nearest and furthest and how long the search took.

// Points are random (-n) or read form a CSF file (-f).
// Distance is either taken as a striaght line (-d std) or Manhattan (-d man).
// It runs out one thread (-s) or several with OpenMP (-p).
// The search checks each pair twice using the default (-a naive) or once using the optimised (-a half).
// or skips most pairs with a grid and 'convex hull' (-a fast).

// Usage:
//     nearest_furthest -n <count> -d std|man -s|-p [-a naive|half|fast] [-t threads]
//     nearest_furthest -f <file>  -d std|man -s|-p [-a naive|half|fast] [-t threads]

// The number of threads comes from -t or OMP_NUM_THREADS without and the loop shceule from OMP_SCHEDULE.


#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <omp.h>
#include "Distances.h"
#include "Locations.h"
#include "ConvexHull/FastSearch.h"

using std::cout,std::cerr,std::endl;

// Stores the users choices from the command line
struct Settings {
  int count =0;                                // -n: number of random points to make (0 if not given)
  std::string filename;                        // -f: CSV file to read the points from
  DistanceType type = DistanceType::Standard;  // -d: which distance to use (std or man)
  bool parallel = false;                       // -s (serial) or -p (parallel)
  bool half = false;                           // -a: naive (every pair twice) or half (each pair once)
  bool fast = false;                           // -a fast: a grid for the nearest, the convex hull for the furthest
  int threads = 0;                             // -t: number of threads for -p (0 if not given) (OpenMP decides)
};

void printUsage() {
  cerr << "Usage:\n"
       << " nearest_furthest -n <count> -d std|man -s|-p\n"
       << " nearest_furthest -f <file>  -d std|man -s|-p\n"
       << " add -a half to check each pair only once or -a fast to skip majority of pairs\n"
       << " add -t <threads> to pick how many threads -p uses\n";
}

// Fills in settings from the command line. Returns false if it isn't valid.
// Steps through each terminal argument after program name (starting at argv[1])
bool readCommandLine(int argc, char*argv[], Settings&settings) {
  bool serialGiven = false;
  bool parallelGiven = false;

  for (int i=1 ; i<argc; ++i) {
    std::string option = argv[i];
    bool hasValue = (i+1 < argc); // check if other word after to use as its value
    // -n, -f, -d, -a, -t, need a value straight after them
    if ((option == "-n"||option == "-f"||option == "-d"||option == "-a"||option == "-t") && !hasValue) {
      cerr << "!! ERROR -- " <<option<< " needs a value after it !!" << endl;
      return false;
    }

    if (option == "-n" && hasValue) {
      // i++ moves onto the value meaning the loop wont treat it as an option again after
      // atoi turns the text into a number
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
      // chck if value is allowed, then set it
      if (value != "std" && value != "man") {
        cerr << "!! ERROR!!! -- -d must be followed by std or man !!" << endl;
        return false;
      }
      settings.type = (value == "man") ? DistanceType::Manhattan : DistanceType::Standard;
    }
    
    else if (option == "-a" && hasValue) {
      std::string value = argv[++i];
      if (value!="naive" && value!="half" && value!="fast") {
        cerr<< "!! ERROR -- -a must be followed by naive, half or fast !!" <<endl;
        return false;
      }
      settings.fast = (value == "fast");
      settings.half = (value == "half");
    }

    else if (option == "-t" && hasValue) {
      settings.threads = std::atoi(argv[++i]);
      if (settings.threads < 1) {
        cerr << "!! ERROR -- -t needs to be at least 1 thread !!" <<endl;
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
      cerr<< "!! ERROR -- unknown option " <<option<< "!!" <<endl;
      return false;
    }
  }
  
  // count (starting at 0) goes above if -n and fileneame only filled if -f
  bool countGiven = (settings.count >0);
  bool fileGiven = !settings.filename.empty();
  // true if both were given or neither
  if (countGiven == fileGiven) {
    cerr<< "!! ERROR -- give EITHER -n or -f !!" <<endl;
    return false;
  }
  // both makes 0 sense, anything else fine
  if (serialGiven && parallelGiven) {
    cerr<< "!! ERROR -- give EITHER -s or -p !!" <<endl;
    return false;
  }

  settings.parallel = parallelGiven;
  return true;
}


// writes one value per line in same order as points
// so a lines number corresponds to its answers line number (default format 6.sf)
void writeValues(const std::string& filename, const std::vector<double>&values) {
  std::ofstream file(filename);
  if (!file) {   // the file could be open in excel which would lock it in windows
    cerr << "!! ERROR -- can't write " <<filename<< " (is it open in another program?) !!" << endl;
    std::exit(EXIT_FAILURE);
  }
  for (double value : values) {
    file <<value<< '\n';
  }
}

// read options, get points, time search, write and print
int main(int argc, char*argv[]) {
  Settings settings;
  if (!readCommandLine(argc,argv, settings)) {
    printUsage();
    return EXIT_FAILURE;
  }
  // -t sets how many threads the parallel loops use (without, OpenMP decides which is probs all of them)
  if (settings.threads >0) {
    omp_set_num_threads(settings.threads);
  }

  std::vector<Point> points;
  if (!settings.filename.empty()) {
    points = readLocations(settings.filename);
  }
  else {
    points = generateRandomLocations(settings.count);
  }
  // file could have less than 2 points, then no neighbour so ERROR
  if (points.size() <2) {
    cerr << "!! ERROR -- need at least 2 points, but found only " <<points.size()<< "!!" << endl;
    return EXIT_FAILURE;
  }

  // Start timing.
  // Only search and two means are timed, not reading or writing the files, so times compare searches themselfvs.
  // steady_clock never goes backwards -- omp_get_wtime gave negative times with MinGW
  // All this ensures time taken cant come out as negative
  auto startTime = std::chrono::steady_clock::now();
  
  std::vector<double> nearest;
  std::vector<double> furthest;

  // -a half picks search tool to only work out each pair once
  // -a fast picks the one that uses a grid and the 'convex hull' to skip most pairs
  // settings.parralel goes in too meaning same function does both -s and -p
  if (settings.fast) {
    findNeighboursFast(points, settings.type, settings.parallel, nearest, furthest);
  }
  else if (settings.half) {
    findNeighboursHalf(points, settings.type, settings.parallel, nearest, furthest);
  }
  else {
    findNeighbours(points, settings.type, settings.parallel, nearest, furthest);
  }
  double meanNearest = mean(nearest, settings.parallel);
  double meanFurthest = mean(furthest, settings.parallel);
  //take time on clock from the start time -  Stop timing now() minus startTime gives a duration
  double timeTaken = std::chrono::duration<double>(std::chrono::steady_clock::now() -startTime).count(); 

  // done after timer, so writing files insnt counted
  writeValues("nearest.csv", nearest);
  writeValues("furthest.csv", furthest);

  // The output lines exactly as the breif specifies.
  // 'fixed' and 'setprecision(3)' give time to 3.dp, because its set after means are printed, they keep their default 6.sf
  cout << "Mean distance to nearest neighbour: "  <<meanNearest  <<endl;
  cout << "Mean distance to furthest neighbour: " <<meanFurthest <<endl;
  cout << std::fixed << std::setprecision(3);
  cout << "Time taken: " << timeTaken << " seconds" << endl;

  return EXIT_SUCCESS;
}