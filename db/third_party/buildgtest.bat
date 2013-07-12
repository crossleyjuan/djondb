#!/bin/sh

mkdir gtest
cd gtest
unzip -o ../gtest-1.6.0.zip 
cd gtest-1.6.0
./configure
make

cp -f lib/.libs/libgtest*.a ../../libs/
cp -fR include/* ../../includes/

