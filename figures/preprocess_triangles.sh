#!/bin/bash

set -ex

pdpcComputeMultiScaleFeatures -i ../data/triangles.ply -o data_triangles -v  
pdpcSegmentation -i ../data/triangles.ply -s data_triangles_scales.txt -f data_triangles_features.bin -o data_triangles -v 
