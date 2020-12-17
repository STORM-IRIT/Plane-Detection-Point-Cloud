#!/bin/bash

set -ex

pdpcComputeMultiScaleFeatures -i ../data/stairs.ply -o data_09b -v  
pdpcSegmentation -i ../data/stairs.ply -s data_09b_scales.txt -f data_09b_features.bin -o data_09b -v 
pdpcPostProcess -i ../data/stairs.ply -s data_09b_seg.bin -r data_09b_reg.bin -c data_09b_comp.bin -o fig_09b -col -v -pers 15 20 25
