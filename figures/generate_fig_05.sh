#!/bin/bash

set -ex

mkdir -p fig_05
../build/pdpcPostProcess -i ../data/triangles.ply -s data_triangles_seg.txt -c data_triangles_comp.txt -o fig_05/fig_05 -col -v -range 20 24 25 30 40 42
