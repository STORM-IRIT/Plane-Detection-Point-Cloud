#!/bin/bash

set -ex

pdpcComputeMultiScaleFeatures -i ../data/lans.ply -o data_09c -v  
pdpcSegmentation -i ../data/lans.ply -s data_09c_scales.txt -f data_09c_features.bin -o data_09c -v 
pdpcPostProcess -i ../data/lans.ply -s data_09c_seg.bin -r data_09c_reg.bin -c data_09c_comp.bin -o fig_09c -col -v -pers 5 10 15
