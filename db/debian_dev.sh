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
find debian_dev -type f | xargs chmod 644
find debian_dev -type d | xargs chmod 755

#http://www.debian.org/doc/debian-policy/ch-files.html#s-libraries
strip --strip-unneeded debian_dev/usr/lib/*.so*

fakeroot dpkg-deb --build debian_dev djondb_dev.deb
lintian djondb_dev.deb

debfile="djondb_dev_`uname`_`uname -i`.deb"
mv djondb_dev.deb $debfile

if [ ! -z "${DIR}" ]; 
then
	cp $debfile $DIR
fi
