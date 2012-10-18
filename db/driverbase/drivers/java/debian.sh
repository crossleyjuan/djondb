#!/bin/sh

## find packages
## dpkg -S 

# ldd ./djondb

# dpkg -S /bin/djondbd
# apt-cache showpkg libxss1
find debian -type f | xargs chmod 644
find debian -type d | xargs chmod 755
chmod +x debian/usr/bin/*

#http://www.debian.org/doc/debian-policy/ch-files.html#s-libraries
#strip debian/usr/bin/djondbd
#strip debian/usr/bin/djon-shell

strip --strip-unneeded debian/usr/lib/*.so*

fakeroot dpkg-deb --build debian djondb.deb
lintian djondb.deb

