#!/bin/sh

sh update.sh
rm -rf ruby
mkdir ruby

swig2.0 -c++ -ruby -outdir ruby -o ruby/djonrubydriver.cpp driver-ruby.i

cd ruby
cp ../*.h .
cp ../extconf.rb .
ruby extconf.rb  --with-djondb-dir=`pwd`/../..

cp ../djondb.gemspec .

make

mkdir lib
cp djondb.so lib/

gem build djondb.gemspec
# sudo make install
