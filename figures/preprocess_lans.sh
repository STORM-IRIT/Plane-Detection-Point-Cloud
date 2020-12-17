#!/bin/bash

set -ex

pdpcComputeMultiScaleFeatures -i ../data/lans.ply -o data_lans -v  
pdpcSegmentation -i ../data/lans.ply -s data_lans_scales.txt -f data_lans_features.bin -o data_lans -v 
