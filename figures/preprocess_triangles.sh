#!/bin/bash

set -ex

../build/pdpcComputeMultiScaleFeatures -i ../data/triangles.ply -o data_triangles -v  
../build/pdpcSegmentation -i ../data/triangles.ply -s data_triangles_scales.txt -f data_triangles_features.txt -o data_triangles -v 
