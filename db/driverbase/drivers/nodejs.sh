#!/bin/sh

sh update.sh
sh update-nodejs.sh

while getopts s:j:d:u o
   do case "$o" in
		j)  JAVA="$OPTARG";;
		d)  DIR="$OPTARG";;
	   u)  UPLOAD="true";;
		\?)  echo "Usage: $0 -j <java_home> -d output_dir -u" && exit 1;;
	esac
done

cd nodejs

if [ ! -z "${UPLOAD}" ]; 
then
	npm publish
fi
