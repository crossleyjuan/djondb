#!/bin/sh

sh update.sh

rm -rf php
mkdir php

cp ../../obj/usr/lib/libdjon-client.0.dylib ../../obj/usr/lib/libdjon-client.dylib ../../obj/usr/lib/libdjon-client.la php/

swig -c++ -php -outdir php -o php/djonphpdriver.cpp driver.i

cp config.m4 php
cd php

phpize

./configure --enable-djonwrapper
make


zipfile="djondb_phpext_`uname`_`uname -m`.zip"

zip $zipfile test.php modules/djonwrapper.so djonwrapper.php
