#!/bin/bash

set -ex

pdpcComputeMultiScaleFeatures -i ../data/lans.ply -o data_15b -v  
pdpcSegmentation -i ../data/lans.ply -s data_15b_scales.txt -f data_15b_features.bin -o data_15b -v 
pdpcPostProcess -i ../data/lans.ply -s data_15b_seg.bin -r data_15b_reg.bin -c data_15b_comp.bin -o fig_15b -col -v -scales 1 2 3 4 5 6 7 8 9 10 12 15 18 20 22 25 30
