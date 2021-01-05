#!/bin/bash

set -ex

# CGAL is the only required dependency 
sudo apt-get install libcgal-dev

# download, compile and install the code in /usr/local/bin
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local ..
make -j 
sudo make install
cd ..

# generate figures
cd figures
./generate.sh
