#!/bin/bash
#Usage: ./partitiondata.sh [file] [num] [host]
#file path must be an absolute path
#Warning: it will cover the file named "data"+"_"
#Warning: host file should not contain itself
#[num] is the number of partition data, it must be more than the Worker number of LIBBLE-PS
rm -r $1"_"
mkdir $1"_"
g++ partitiondata.cpp -o partitiondata -std=c++11
echo "partition data ..."
./partitiondata $1 $2
pssh -h $3 rm -r $1"_"
str=${1%/*}
echo "distribute data ..."
pscp -h $3 -r $1"_" $str