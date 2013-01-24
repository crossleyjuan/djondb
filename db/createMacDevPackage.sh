#!/bin/sh

cd mac
mkdir -p djonpkgdev/usr/lib
mkdir -p djonpkgdev/usr/include
rm djonpkgdev/usr/lib/*
rm djonpkgdev/usr/include/*
cp -R ../obj/usr/lib/libdjon-client* djonpkgdev/usr/lib
cp -R ../obj/usr/include/djondb djonpkgdev/usr/include

cd djonpkgdev
cp /usr/lib/libant* usr/lib/
cp /usr/lib/libv8* usr/lib/
