#!/bin/sh

c="."
for i in jaguar-client/dist/* ; do
##  j=?
  c="$c:$i"
done
for i in jaguar-client/dist/lib/* ; do
##  j=?
  c="$c:$i"
done

echo $c
## -agentlib:jdwp=transport=dt_socket,address=9797,server=y,suspend=y
java -classpath $c com.sinco.client.Launcher


