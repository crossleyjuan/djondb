#!/bin/sh

echo "<<<<<  Executing autoreconf  >>>>>"

autoreconf --install --force

rm -rf obj
mkdir obj
cd obj
../configure --prefix=/usr
make
make DESTDIR=`pwd` install
rm usr/bin/test*
rm -rf ../debian/usr
mv usr ../debian/
# cp /usr/lib/x86_64-linux-gnu/libantlr3c-3.2.so.0 ../debian/usr/lib
# cp /usr/lib/libv8.so.3.7.12.22 ../debian/usr/lib

cd ..
sh debian.sh $@

mkdir -p debian_dev/usr/lib
cp debian/usr/lib/libdjon-client* debian_dev/usr/lib/

sh debian_dev.sh $@
# scp djondb.deb crossleyjuan@d-jon.com:html/downloads/djondb.deb

cd driverbase/drivers

./php.sh $@
./java.sh $@
cd nodejs
npm publish
