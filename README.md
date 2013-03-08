djondb
======

Welcome to djondb, this is a quick readme with very simple compilation instructions. 

Compilation
===========

As any other standard linux application djondb can be compiled using configure/make/make install steps, to create
the configuration file use the autoreconf command line. Here're the steps to do the compilation:

cd db
autoreconf --install --force
mkdir obj
cd obj
../configure --prefix=/usr
make
sudo make install

Release Compilation
===================

To make the release compilation even easier there's an script at the root of the folder db for each operating system:

Linux:   release.sh -j <java path> -o <output dir>
Mac:   release-mac.sh
Windows: release-windows.bat

These scripts will compile the server and the drivers and will place the binaries in a destination folder

Drivers Compilation
======================

At the base drivers folder db/driverbase/drivers you will find an script for each driver, this script will generate/copy/update all
the files required for each driver.

csharp.sh 
java.sh -j <java path> -d <outputdir> 
nodejs.sh 
php.sh -d <outputdir>
ruby.sh
python/python.sh



