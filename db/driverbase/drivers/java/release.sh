#!/bin/sh
while getopts s:j:d:u o
   do case "$o" in
	   s)  SUFFIX="$OPTARG";;
		\?)  echo "Usage: $0 -jdkhome <java_home> [-s suffix]" && exit 1;;
	esac
done

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
jarfile="djondb_client_`uname`_`uname -m`${SUFFIX}.jar"
mv java/dist/lib/djondb_java.jar dist/$jarfile
#scp dist/$jarfile  crossleyjuan@djondb.com:html/downloads/$jarfile

