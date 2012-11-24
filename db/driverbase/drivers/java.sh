#!/bin/sh

JAVA=$JAVA_HOME
while getopts j:d: o
   do case "$o" in
		j)  JAVA="$OPTARG";;
		d)  DIR="$OPTARG";;
		\?)  echo "Usage: $0 -jdkhome <java_home>" && exit 1;;
	esac
done

if [ -z "${JAVA}" ];
then
	echo you need to specify the java home directory using -j or setting the JAVA_HOME environment variable
	exit 1;
fi

export JAVA_HOME=$JAVA
sh update.sh
swig2.0 -outdir java/java/src/djondb -o java/native/javadriver_wrap.cpp -c++ -java -package djondb driver.i

cd java
./release.sh

if [ ! -z "${DIR}" ]; 
then
	cp dist/* $DIR
fi
