#!/bin/bash

# Keep at 11 due to issues with ranges in gcc12 (?)
export CC=/usr/bin/gcc-11 &&
export CXX=/usr/bin/g++-11 &&

cmake -H. -Bbuild &&
cmake --build build -- -j5
