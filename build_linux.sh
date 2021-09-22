#!/bin/bash

export CC=/usr/bin/gcc-11 &&
export CXX=/usr/bin/g++-11 &&

rm -rf Release/ &&
cd Fronter &&
./build_linux.sh &&
mv Release ../ &&
cd ../EU4toV3 &&
rm -rf build &&
rm -rf Release-Linux &&
cmake -H. -Bbuild &&
cmake --build build -- -j40  &&
mv Release-Linux ../Release/EU4ToVic3 &&
cd .. &&

cp EU4toV3/Data_Files/*yml Release/Configuration/ &&
cp EU4toV3/Data_Files/fronter*txt Release/Configuration/ &&

tar -cjf EU4ToVic3-release.tar.bz2 Release
