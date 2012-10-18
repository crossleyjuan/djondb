#!/bin/sh

JAVA=$JAVA_HOME
while getopts j: o
   do case "$o" in
	    j)  JAVA="$OPTARG";;
       \?)  echo "Usage: $0 -j <java_home>" && exit 1;;
   esac
done

if [ -z "${JAVA}" ];
then
	echo you need to specify the java home directory using -j or setting the JAVA_HOME environment variable
	exit 1;
fi

$JAVA/bin/javac -classpath java/dist/djondb_client_Linux_x86_64.jar:. Test.java
$JAVA/bin/java -classpath java/dist/djondb_client_Linux_x86_64.jar:. Test
