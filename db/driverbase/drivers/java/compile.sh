#!/bin/sh
echo "native JNI compilation"

cd native

mkdir obj
cd obj
make
make DESTDIR=`pwd` install

cd ../../java
echo "java compilation"
ant

cd ..
rm -rf dist
mkdir dist
jarfile="djondb_client_`uname`_`uname -m`.jar"
mv java/dist/lib/djondb_java.jar dist/$jarfile
#scp dist/$jarfile  crossleyjuan@djondb.com:html/downloads/$jarfile

