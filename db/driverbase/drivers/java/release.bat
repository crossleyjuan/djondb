cd ..\..\java
ant clean
ant

cd ..
rm -rf dist
mkdir dist
jarfile="djondb_client_win32.jar"
move java\dist\lib\djondb_java.jar dist\%jarfile%
#scp dist\$jarfile  crossleyjuan@djondb.com:html\downloads\%jarfile%

