#!/bin/sh

sh update.sh
rm -rf ruby
mkdir ruby

swig2.0 -c++ -ruby -outdir ruby -o ruby/djonrubydriver.cpp driver.i

cd ruby
cp ../*.h .
cp ../extconf.rb .
ruby extconf.rb  --with-djondb-dir=`pwd`/../..
make
sudo make install
