#!/bin/bash

set -ex

pdpcComputeMultiScaleFeatures -i ../data/stairs.ply -o data_stairs -v  
pdpcSegmentation -i ../data/stairs.ply -s data_stairs_scales.txt -f data_stairs_features.txt -o data_stairs -v 
