#!/bin/bash

rm -rf build
mkdir build
cd build 
cmake -DENABLE_TESTING=ON ..
make 
make test


