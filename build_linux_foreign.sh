#!/bin/bash

cmake -H. -Bbuild &&
cmake --build build -- -j5
