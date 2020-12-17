#!/bin/bash

set -ex

pdpcPostProcess -i ../data/triangles.ply -s data_triangles_seg.bin -r data_triangles_reg.bin -c data_triangles_comp.bin -o fig_05 -col -v -range 20 24 25 30 40 42
