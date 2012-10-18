#!/bin/sh

cd mac
rm -rf djonpkg
mkdir djonpkg
cp -R ../obj/usr djonpkg/usr

cd djonpkg
rm usr/bin/test*
rm usr/lib/*.a
rm usr/lib/*.la
cp /usr/lib/libant* usr/lib/
cp /usr/lib/libv8* usr/lib/
