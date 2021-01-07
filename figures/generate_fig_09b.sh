#!/bin/bash

set -ex

../build/pdpcPostProcess -i ../data/stairs.ply -s data_stairs_seg.txt -c data_stairs_comp.txt -o fig_09b -col -v -pers 15 20 25
