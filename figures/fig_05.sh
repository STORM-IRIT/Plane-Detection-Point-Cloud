#!/bin/bash

set -ex

input=../data/triangles.ply

pdpcComputeMultiScaleFeatures -i $input -o data_05 -v  
pdpcSegmentation -i $input -s data_05_scales.txt -f data_05_features.bin -o data_05 -v 
pdpcPostProcess -i $input -s data_05_seg.bin -r data_05_reg.bin -c data_05_comp.bin -o fig_05 -col -v -range 24 26 30 32 48 50

