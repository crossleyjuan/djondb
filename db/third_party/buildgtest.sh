#!/bin/sh

mkdir gtest
cd gtest
unzip ../gtest-1.6.0.zip 
./configure
make

cp .libs/libgtest*.a ../libs/
cp include/*.h ../includes/

