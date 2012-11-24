#!/bin/sh

while getopts j:d: o
   do case "$o" in
		d)  DIR="$OPTARG";;
		\?)  echo "Usage: $0 -d dist_dir" && exit 1;;
	esac
done

## find packages
## dpkg -S 

# ldd ./djondb

# dpkg -S /bin/djondbd
# apt-cache showpkg libxss1
find debian -type f | xargs chmod 644
find debian -type d | xargs chmod 755
chmod +x debian/usr/bin/*

#http://www.debian.org/doc/debian-policy/ch-files.html#s-libraries
strip debian/usr/bin/djondbd
strip debian/usr/bin/djon-shell

strip --strip-unneeded debian/usr/lib/*.so*

fakeroot dpkg-deb --build debian djondb.deb
lintian djondb.deb

debfile="djondb_`uname`_`uname -i`.deb"
mv djondb.deb $debfile

if [ ! -z "${DIR}" ]; 
then
	cp $debfile $DIR
fi
