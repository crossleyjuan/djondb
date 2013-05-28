#!/bin/sh

echo "<<<<<  Executing autoreconf  >>>>>"

while getopts s:j:d:u o
   do case "$o" in
		j)  JAVA="$OPTARG";;
		d)  DIR="$OPTARG";;
	   u)  UPLOAD="true";;
	   s)  SUFFIX="$OPTARG";;
		\?)  echo "Usage: $0 [-j java_home] [-d output_dir] [-u] [-s suffix]" && exit 1;;
	esac
done

cd includes
./update.sh
cd ..

if [ ! -z "${DIR}" ]; 
then
	mkdir -p $DIR
fi

rm -rf build
mkdir build
cd build
cmake ..
make
cpack -G DEB

make DESTDIR=`pwd` install
sudo make install

debfile="djondb_`uname`_`uname -i`${SUFFIX}.deb"
deb=`find djondb*.deb`
mv $deb $debfile

if [ ! -z "${DIR}" ]; 
then
	cp $debfile $DIR
fi

cd ..

cd driverbase/drivers

./update.sh
./php.sh $@

./java.sh $@
./csharp.sh $@

cd python
./python.sh
cd ..

sh nodejs.sh $@

