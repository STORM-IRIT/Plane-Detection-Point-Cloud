#!/bin/bash

set -ex

../build/pdpcComputeMultiScaleFeatures -i ../data/lans.ply -o data_lans -v  
../build/pdpcSegmentation -i ../data/lans.ply -s data_lans_scales.txt -f data_lans_features.txt -o data_lans -v 
