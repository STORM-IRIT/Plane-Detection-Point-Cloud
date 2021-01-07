#!/bin/bash

set -ex

mkdir -p fig_15b
../build/pdpcPostProcess -i ../data/lans.ply -s data_lans_seg.txt -c data_lans_comp.txt -o fig_15b/fig_15b -col -v -scales 5 15 20 25
