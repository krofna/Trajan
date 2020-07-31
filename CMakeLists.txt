cmake_minimum_required(VERSION 3.0)
project(dagmatch CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -O2 -march=native -lglog -lortools")
 
find_package(Eigen3 REQUIRED NO_MODULE)
find_package(Threads REQUIRED)

include_directories(
  ${PROJECT_SOURCE_DIR}
)
 
add_executable(dagmatch_lp
  src/main.cpp
  src/Graph.cpp
  src/LP.cpp
  src/AntichainConstraint.cpp
  src/Constraint.cpp
  src/IndependentSetConstraint.cpp
  src/CrossingConstraint.cpp
  src/read_csv.cpp
  geno/augmentedLagrangian.cpp
  geno/lbfgsb.cpp
  geno/lineSearch.cpp)
add_executable(dagmatch_fpt
  src/match.cpp
  src/read_csv.cpp)
add_executable(dagmatch_greedy
  src/greedy.cpp
  src/read_csv.cpp)

target_link_libraries(dagmatch_lp Eigen3::Eigen Threads::Threads)
