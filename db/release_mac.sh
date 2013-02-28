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


sh createMacPackage.sh

# cd ../mac
# sh macpackage.sh

# rm djondb_mac/*
# cp djondb.app/* djondb_mac/
# rm djondb_mac/Info*
# rm djondb_mac/*.png
# tar cvfz dist/djondb_0.1.tar.gz  djondb_mac/*

# scp dist/djondb_0.1.tar.gz crossleyjuan@d-jon.com:html/downloads/djondb_0.1.tar.gz
 
