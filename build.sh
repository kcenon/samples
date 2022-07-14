#!/bin/bash
rm -rf bin
rm -rf build

mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE="../../vcpkg/scripts/buildsystems/vcpkg.cmake" -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=YES -DUSE_SWIG_INTERFACE=ON
make -B
export LC_ALL=C
unset LANGUAGE

cd ..

cp -R ./build/bin/* ./bin

./bin/unittest

rm -rf build