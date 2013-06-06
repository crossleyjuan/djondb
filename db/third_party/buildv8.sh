#!/bin/sh

tar xvfz v8.tar.gz
cd v8

python build/gyp_v8 -Dcomponent=static_library
make native
