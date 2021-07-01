cmake_minimum_required(VERSION 3.8)
project(LIBBLE_PS)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
SET(CMAKE_CXX_COMPILER mpicxx)
SET(CMAKE_C_COMPILER mpicc)

include_directories(src)

include(app/lr/CMakeLists.txt)
include(app/svm/CMakeLists.txt)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/app/bin)
