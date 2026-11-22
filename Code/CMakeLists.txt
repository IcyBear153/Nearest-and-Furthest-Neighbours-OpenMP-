# Build file for CMake. Works with Visual Studio or with Ninja and MinGW
# From Code folder, run:
#   cmake -S . -B build
#   cmake --build build --config Release
cmake_minimum_required(VERSION 3.12)
project(NearestFurthest LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17) # C++17 is needed (to allow using std::cout, std::cerr, std::endl; on one line)
set(CMAKE_CXX_STANDARD_REQUIRED ON) # Fail immediately if C++17 not supported instead of falling back to older version

# Optimised build by default - unoptimisedbuild is slower making timings meaningless
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release)
endif()

# Build every .cpp file in this folder into one program
file(GLOB SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
add_executable(nearest_furthest ${SOURCES})

# Enable OpenMP for whatever compiler is used
find_package(OpenMP REQUIRED)
target_link_libraries(nearest_furthest PRIVATE OpenMP::OpenMP_CXX)
