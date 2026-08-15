#!/usr/bin/env bash

set -e

export PKG_CONFIG_PATH=/usr/lib/pkgconfig:$PKG_CONFIG_PATH

rm -rf build/
mkdir build
cd build

cmake ..
cmake --build .
