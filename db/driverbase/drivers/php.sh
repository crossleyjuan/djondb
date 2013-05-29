#!/bin/sh

while getopts s:j:d:u o
   do case "$o" in
		d)  DIR="$OPTARG";;
	   u)  UPLOAD="true";;
	   s)  SUFFIX="$OPTARG";;
		\?)  echo "Usage: $0 -d dist_dir [-s suffix]" && exit 1;;
	esac
done


#rm -rf php
mkdir php

sh update-php.sh
OS=`uname -s`
if test "$OS" = "Darwin"; then
cp ../../build/usr/lib/libdjon-client.dylib ../../build/usr/lib/libdjon-client.dylib php/
else
cp ../../build/usr/lib/libdjon-client.so php/
fi

swig -c++ -php -outdir php -o php/djonphpdriver.cpp driver.i

cp config.m4 php
cd php

phpize

./configure --enable-djonwrapper
make


zipfile="djondb_phpext_`uname`_`uname -m`${SUFFIX}.zip"

zip $zipfile test.php modules/djonwrapper.so djonwrapper.php

if [ ! -z "${DIR}" ]; 
then
	cp $zipfile $DIR
fi
