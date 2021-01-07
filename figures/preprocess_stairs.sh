#!/bin/bash

set -ex

../build/pdpcComputeMultiScaleFeatures -i ../data/stairs.ply -o data_stairs -v  
../build/pdpcSegmentation -i ../data/stairs.ply -s data_stairs_scales.txt -f data_stairs_features.txt -o data_stairs -v 
