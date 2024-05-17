cmake_minimum_required(VERSION 3.10)

project(PathFindingKingsBoard)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(PathFindingKingsBoard Board.cpp KingsManager.cpp Pathfinder.cpp main.cpp)

include_directories(include)