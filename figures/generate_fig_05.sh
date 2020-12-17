#!/bin/bash

set -ex

pdpcComputeMultiScaleFeatures -i ../data/triangles.ply -o data_05 -v
pdpcSegmentation -i ../data/triangles.ply -s data_05_scales.txt -f data_05_features.bin -o data_05 -v 
pdpcPostProcess -i ../data/triangles.ply -s data_05_seg.bin -r data_05_reg.bin -c data_05_comp.bin -o fig_05 -col -v -range 20 24 25 30 40 42
