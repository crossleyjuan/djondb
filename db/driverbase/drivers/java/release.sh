#!/bin/sh
echo "native JNI compilation"

cd native

echo "<<<<<  Executing autoreconf  >>>>>"

autoreconf --install --force

rm -rf obj
mkdir obj
cd obj
../configure --prefix=/usr
make
make DESTDIR=`pwd` install

cd ../../java
ant clean
ant

cd ..
rm -rf dist
mkdir dist
jarfile="djondb_client_`uname`_`uname -m`.jar"
mv java/dist/lib/djondb_java.jar dist/$jarfile
#scp dist/$jarfile  crossleyjuan@djondb.com:html/downloads/$jarfile

