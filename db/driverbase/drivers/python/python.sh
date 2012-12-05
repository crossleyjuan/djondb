#!/bin/sh

cd ..
#executes update to ensure .h files
sh update.sh
cd python
cp ../*.h .

rm -rf output
mkdir output
mkdir output/include

cp ../*.h output/include/

OS=`uname -s`
if test "$OS" = "Darwin"; then
cp ../../../obj/usr/lib/libdjon-client.0.dylib ../../obj/usr/lib/libdjon-client.dylib output/
else
cp ../../../obj/usr/lib/libdjon-client.la output/
fi

swig2.0 -c++ -python -outdir output -o output/djonpythondriver.cpp driver-python.i

cp setup.py output/
cp MANIFEST.in output/

cd output
python setup.py register
#python setup.py build_ext --inplace
python setup.py sdist upload
#python setup.py bdist_dumb upload
#python setup.py bdist_dumb upload

