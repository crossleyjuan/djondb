djondb
======

Welcome to djondb, this is a quick readme with very simple compilation instructions. 

Third party Packages
=================

djondb uses some external libraries that need to be installed prior compilation, these are:

* cmake g++ make
* libv8-dev: Google's V8 javascript engine
* libantlr3c-dev: Antlr
* libuuid1
* gtest

djondb source comes with copy of the antlr, v8 and gtest libraries in the folder db/third_party if
you are in linux or OSX you can use "make dependencies" to build the required libraries as follows:

   cd db
   make dependencies

This command will execute the scripts: third_party/buildantlr.sh third_parth/buildv8.sh and third_party/buildgtest.sh.
    
Compilation
===========

As any other standard linux application djondb can be compiled using cmake/make/make install steps, as follows:

    cd db
    mkdir build
    cd build
    cmake ..
    make
    sudo make install

To compile djondb with debugging symbols you could use: cmake .. -DCMAKE_BUILD_TYPE=Debug, if you already executed cmake command
you will need to erase the contents of the folder prior executing the cmake.

Release Compilation
===================

To make the release compilation even easier there's an script at the root of the folder db for each operating system:

* Linux:   release.sh -j <java path> -o <output dir>
* Mac:     release-mac.sh
* Windows: release-windows.bat

These scripts will compile the server and the drivers and will place the binaries in a destination folder

Drivers Compilation
======================

At the base drivers folder db/driverbase/drivers you will find an script for each driver, this script will generate/copy/update all
the files required for each driver.

* csharp.sh 
* java.sh -j <java path> -d <outputdir> 
* nodejs.sh 
* php.sh -d <outputdir>
* ruby.sh
* python/python.sh


