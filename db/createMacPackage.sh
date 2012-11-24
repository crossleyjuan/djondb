#!/bin/sh

cd mac
rm djonpkg/usr/bin/*
rm djonpkg/usr/lib/*
cp -R ../obj/usr djonpkg/usr

cd djonpkg
rm usr/bin/test*
rm usr/lib/*.a
rm usr/lib/*.la
rm -rf usr/include/djon-*
cp /usr/lib/libant* usr/lib/
cp /usr/lib/libv8* usr/lib/
