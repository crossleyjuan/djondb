#!/bin/sh

echo "<<<<<  Executing autoreconf  >>>>>"

while getopts s:j:d:u o
   do case "$o" in
		j)  JAVA="$OPTARG";;
		d)  DIR="$OPTARG";;
	   u)  UPLOAD="true";;
	   s)  SUFFIX="$OPTARG";;
		\?)  echo "Usage: $0 [-j java_home] [-d output_dir] [-u] [-s suffix]" && exit 1;;
	esac
done

autoreconf --install --force

if [ ! -z "${DIR}" ]; 
then
	mkdir -p $DIR
fi

rm -rf obj
mkdir obj
cd obj
../configure --prefix=/usr
make
make DESTDIR=`pwd` install
rm usr/bin/test* rm -rf ../debian/usr
cp -R usr ../debian/
# cp /usr/lib/x86_64-linux-gnu/libantlr3c-3.2.so.0 ../debian/usr/lib
# cp /usr/lib/libv8.so.3.7.12.22 ../debian/usr/lib

cd ..
sh debian.sh $@

mkdir -p debian_dev/usr/lib
cp debian/usr/lib/libdjon-client* debian_dev/usr/lib/

sh debian_dev.sh $@
# scp djondb.deb crossleyjuan@d-jon.com:html/downloads/djondb.deb

cd driverbase/drivers

./update.sh
./php.sh $@
./java.sh $@
./csharp.sh $@

cd python
./python.sh
cd ..

sh nodejs.sh $@

