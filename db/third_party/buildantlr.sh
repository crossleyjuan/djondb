#!/bin/sh

tar xvfz libantlr3c-3.4.tar.gz
cd libantlr3c-3.4
MACHINE=`uname -m`
X64="x86_64"

echo ${MACHINE}
if [ "${MACHINE}" = "${X64}" ];
then
	options="--enable-64bit"
else
	options=""
fi

options="--enable-64bit"
./configure $options
make
cp .libs/libantlr3c.a ../libs

cp -f include/*.h ../includes/

