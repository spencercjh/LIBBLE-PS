cmake_minimum_required(VERSION 3.19)
project(LIBBLE_PS)

find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})
SET(CMAKE_CXX_COMPILER mpicxx)
SET(CMAKE_C_COMPILER  mpicc)
set(CMAKE_CXX_STANDARD 11)

include_directories(src)
include_directories(src/**)

include(app/lr/CMakeLists.txt)
include(app/svm/CMakeLists.txt)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/app/bin)

add_executable(partition_data partitiondata.cpp)