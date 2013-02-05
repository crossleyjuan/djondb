#!/bin/sh

DESTDIR=`pwd`
cd /home/cross/workspace/djondb_src/db/
tar cvfz $DESTDIR/djondb-0.2.3.tar.gz -X $DESTDIR/exclude.txt *
