#!/bin/sh

tar xvfz v8.tar.gz
cd v8

python build/gyp_v8 -Dcomponent=static_library
make native
cp out/native/obj.target/tools/gyp/libv8*.a ../libs/
cp include/*.h ../includes/
