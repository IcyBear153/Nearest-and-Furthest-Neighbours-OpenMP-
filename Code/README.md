# Nearest and Furthest Neighbours (OpenMP)

Finds each point's nearest and furthest neighbour in a 1x1 square, in serial or in parallel. See the report for details.

## Build (from `Code`)

```
cmake -S . -B build
cmake --build build --config Release
```

Needs CMake 3.12+ and a C++17 compiler with OpenMP.

Or without CMake:

```
g++ -O2 -fopenmp *.cpp ConvexHull/FastSearch.cpp -o nearest_furthest
```

## Run

```
nearest_furthest -n <count> | -f <file>  -d std|man  -s|-p  [-a naive|half|fast]  [-t threads]
```

Example: `build/nearest_furthest -f ../100000locations.csv -d std -p`
(the program is in `build\Release\` instead if CMake used Visual Studio)

Results go to `nearest.csv` and `furthest.csv`. Set the loop schedule with `OMP_SCHEDULE` (e.g. `$env:OMP_SCHEDULE = "dynamic"`).

## Graphs (`Postprocessing`, needs NumPy and Matplotlib)

- `plot_histograms.py std|man` - histograms from `nearest.csv` and `furthest.csv`
- `plot_schedules.py <csv files>` - speed-up against threads for each schedule
- `ConvexHull/scaling.py` - time against number of points (run from `Code` after building)