#!/bin/sh

tar xvfz libantlr3c-3.4.tar.gz
cd libantlr3c-3.4
MACHINE=`uname -i`
X64="x86_64"

echo ${MACHINE}
if [ "${MACHINE}" = "${X64}" ];
then
	options="--enable-64bit"
else
	options=""
fi

OS=`uname -s`
if [ "${OS}" = "Linux" ];
then
	options="${options} --with-pic"
fi

echo "Options: ${options}"

./configure $options
make
cp .libs/libantlr3c.a ../libs

cp -f antlr3config.h ../includes/
cp -f include/*.h ../includes/

