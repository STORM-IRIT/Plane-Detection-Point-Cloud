#!/bin/bash

set -ex

pdpcPostProcess -i ../data/stairs.ply -s data_stairs_seg.bin -r data_stairs_reg.bin -c data_stairs_comp.bin -o fig_09b -col -v -pers 15 20 25
