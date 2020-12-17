#!/bin/bash

set -ex

./preprocess_triangles.sh
./generate_fig_05.sh

./preprocess_stairs.sh
./generate_fig_09b.sh

./preprocess_lans.sh
./generate_fig_09c.sh
./generate_fig_15b.sh
