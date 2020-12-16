#!/bin/bash

set -ex

input=../data/lans.ply

pdpcComputeMultiScaleFeatures -i $input -o data_15b -v  
pdpcSegmentation -i $input -s data_15b_scales.txt -f data_15b_features.bin -o data_15b -v 
pdpcPostProcess -i $input -s data_15b_seg.bin -r data_15b_reg.bin -c data_15b_comp.bin -o fig_15b -col -v -scales 5 15 20 25

