#!/bin/sh

sh update.sh

rm -rf php
mkdir php

OS=`uname -s`
if [ OS eq 'Darwin' ];
then
cp ../../obj/usr/lib/libdjon-client.0.dylib ../../obj/usr/lib/libdjon-client.dylib php/
else
cp ../../obj/usr/lib/libdjon-client.la php/
fi

swig2.0 -c++ -php -outdir php -o php/djonphpdriver.cpp driver.i

cp config.m4 php
cd php

phpize

./configure --enable-djonwrapper
make


zipfile="djondb_phpext_`uname`_`uname -m`.zip"

zip $zipfile test.php modules/djonwrapper.so djonwrapper.php
