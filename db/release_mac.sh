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

#rm -rf build
mkdir build
cd build
cmake ..
make
cpack -G PackageMaker

make DESTDIR=`pwd` install
sudo make install

dmgfile="djondb_`uname`_`uname -m`${SUFFIX}.dmg"
dmg=`find djondb*Darwin.dmg`
mv $dmg $dmgfile

if [ ! -z "${DIR}" ]; 
then
	cp $dmgfile $DIR
fi

cd ..

cd driverbase/drivers

./update.sh
./php.sh $@

if [ ! -z "${JAVA}" ]; 
then
	./java.sh $@
fi

cd python
./python.sh
cd ..

sh nodejs.sh $@

