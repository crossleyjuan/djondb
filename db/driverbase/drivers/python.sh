#!/bin/sh

while getopts j:d: o
   do case "$o" in
		d) DIR=$2;;
		\?)  echo "Usage: $0 -d dist_dir" && exit 1;;
	esac
done

sh update.sh

rm -rf python
mkdir python
mkdir python/include

cp *.h python/include/

OS=`uname -s`
if test "$OS" = "Darwin"; then
cp ../../obj/usr/lib/libdjon-client.0.dylib ../../obj/usr/lib/libdjon-client.dylib python/
else
cp ../../obj/usr/lib/libdjon-client.la python/
fi

swig2.0 -c++ -python -outdir python -o python/djonpythondriver.cpp driver-python.i

cp setup.py python/

cd python
python setup.py build_ext --inplace
#python setup.py sdist
#python setup.py bdist_dumb
#pythonize

#./configure --enable-djonwrapper
#make


#zipfile="djondb_pythonext_`uname`_`uname -m`.zip"
#
#zip $zipfile test.python modules/djonwrapper.so djonwrapper.python
#
#if [ ! -z "${DIR}" ]; 
#then
#	cp $zipfile $DIR
#fi
