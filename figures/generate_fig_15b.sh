#!/bin/bash

set -ex

pdpcPostProcess -i ../data/lans.ply -s data_lans_seg.bin -r data_lans_reg.bin -c data_lans_comp.bin -o fig_15b -col -v -scales 5 15 20 25
