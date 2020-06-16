#!/bin/bash
if [ $# -ne 1 ]; then
  echo "Usage: $0 <intel/gnu>"
  exit
fi

module load cmake/3.12.4
module load gcc

if [ "$1" == "gnu" ]; then
  rm -rf build install
  cmake -DCMAKE_BUILD_TYPE=Debug -DUSE_OPENMP=False -DCMAKE_CXX_FLAGS=" -fPIC -O0 -Wall -ftree-vectorize -ftree-vectorizer-verbose=2" -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc -H. -Bbuild
  cd build
  make -j 8 --no-print-directory CXX=mpic++ CC=mpicc
  make install
  cd ../

elif [ "$1" == "intel" ]; then
  rm -rf build install
  cmake -DUSE_OPENMP=TRUE -DCMAKE_CXX_FLAGS=" -g -Wall -qopt-report -fPIC " -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=icpc -DCMAKE_C_COMPILER=icc -H. -Bbuild
  cd build
  make -j 8 CXX=icpc CC=icc 
  make install
  cd ../
fi
