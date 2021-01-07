#!/bin/bash

set -ex

# CGAL is the only required dependency 
sudo apt-get install libcgal-dev

# compile the code
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j 
cd ..

# generate figures
cd figures
./generate.sh
