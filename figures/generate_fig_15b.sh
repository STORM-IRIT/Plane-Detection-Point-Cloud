#!/bin/bash

set -ex

pdpcComputeMultiScaleFeatures -i ../data/lans.ply -o data_15b -v  
pdpcSegmentation -i ../data/lans.ply -s data_15b_scales.txt -f data_15b_features.bin -o data_15b -v 
pdpcPostProcess -i ../data/lans.ply -s data_15b_seg.bin -r data_15b_reg.bin -c data_15b_comp.bin -o fig_15b -col -v -scales 5 15 20 25
