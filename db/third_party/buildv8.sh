#!/bin/sh

tar xvfz v8.tar.gz
cd v8

MACHINE=`uname -i`
X64='x86_64'

echo ${MACHINE}
if [ "${MACHINE}" = "${X64}" ];
then
	python build/gyp_v8 -Dtarget_arch=x64 -Dcomponent=static_library
	make x64.release
else
	python build/gyp_v8 -Dtarget_arch=ia32 -Dcomponent=static_library
	make ia32.release
fi
cp -f `find out -iwholename '*/libv8_base*.a'` ../libs/libv8_base.a
cp -f `find out -iwholename '*/libv8_nosnapshot*.a'`  ../libs/libv8_nosnapshot.a
cp -f `find out -iwholename '*/libv8_snapshot*.a'` ../libs/libv8_snapshot.a
cp -f include/*.h ../includes/

