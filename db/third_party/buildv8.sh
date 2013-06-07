#!/bin/sh

tar xvfz v8.tar.gz
cd v8

python build/gyp_v8 -Dtarget_arch=x64 -Dcomponent=static_library
make x64.release
cp -f out/x64.release/libv8_base*.a ../libs/libv8_base.a
cp -f out/x64.release/libv8_nosnapshot*.a ../libs/libv8_nosnapshot.a
cp -f out/x64.release/libv8_snapshot*.a ../libs/libv8_snapshot.a
cp -f include/*.h ../includes/
