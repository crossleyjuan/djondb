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
#mkdir golang

cd golang
sh update-go.sh
OS=`uname -s`
if test "$OS" = "Darwin"; then
cp ../../../build/usr/lib/libdjon-client.dylib ../../build/usr/lib/libdjon-client.dylib .
else
cp ../../../build/usr/lib/libdjon-client.so .
fi

swig -c++ -go -intgosize 32 -o djongodriver.cpp ../driver-go.i 

gcc -c -fpic -I includes djongodriver.cpp
gcc -shared djongodriver.o -lstdc++ -ldjon-client -o djonwrapper.so
6g djonwrapper.go
6c -I ~/go/src/pkg/runtime/ djonwrapper_gc.c
gopack grc djonwrapper.a djonwrapper.6 djonwrapper_gc.6

exit 1
zipfile="djondb_phpext_`uname`_`uname -m`${SUFFIX}.zip"

zip $zipfile test.php modules/djonwrapper.so djonwrapper.php

if [ ! -z "${DIR}" ]; 
then
	cp $zipfile $DIR
fi
