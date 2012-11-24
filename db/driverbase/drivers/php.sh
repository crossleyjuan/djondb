#!/bin/sh

while getopts j:d: o
   do case "$o" in
		d) DIR=$2;;
		\?)  echo "Usage: $0 -d dist_dir" && exit 1;;
	esac
done

sh update.sh

rm -rf php
mkdir php

OS=`uname -s`
if test "$OS" = "Darwin"; then
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

if [ ! -z "${DIR}" ]; 
then
	cp $zipfile $DIR
fi
