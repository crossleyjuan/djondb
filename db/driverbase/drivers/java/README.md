djondb java driver
=============

One of the great things of djondb is that it provides a base library that
can be used by any driver library, that means we don't need to create any
complicated code to communicate with the server, protocols, etc. Everything
is handled by the native library, and each driver is a wrapper of this base
library.

This project uses JNI to create a wrapper around the native library and the
code in java.

Compilation
-----------

The compilation is simple, the java driver project contains two subprojects:
the native jni code and the java code.

Compiling the native code:

If you're using linux the steps are very simple:

    cd native
    autoreconf --install --force
    mkdir obj
    cd obj
    // Replace with your java directory
    ../configure --jdk-home=/usr/lib/java/java-7-openjdk-amd64 
    make
    make DESTDIR=``pwd`` install
    
Note: be aware that the build.xml used in the java project will look for the binary file at obj/usr/lib/... 
if you need to change the path please change it too at the build.xml compile task

done!, now it's time to compile the java project

    cd java
    ant

The build.xml will copy the native library file into the build directory, this will allow to create a jar
file with the library included in there, simplifying the deployment process.
    
Usage
-----

The full documentation is in: http://djondb.com/javadriver.html but here's a very fast reference to it:

The first step is to initialize the library:

    Loader.initialize();

Be sure to execute this instruction only once. (A static block could do the trick)

Establish a connection:

    Connection con = ConnectionManager.getConnection("localhost");
    if (!con.open()) {
        System.out.println("Could not connect to djondb server");
        System.exit(0);
    }

create a document:

    String document = "{name: 'John', lastName: 'Smith', address: { phone: '555-555-555' } }";

insert it to a database and namespace:

    con.insert("javadb", "customers", document);

find your records back:

    java.util.ArrayList<BSONObj> result = con.find("javadb", "customers");
	 for (java.util.Iterator<BSONObj> i = result.iterator(); i.hasNext(); ) {
           BSONObj obj = i.next();
           System.out.println(obj.getString("name"));
    }

filtering the results:

    java.util.ArrayList<BSONObj> res = con.find("javadb", "customers", "$\"lastName\" == \"Smith\"");

you can do selects to retrieve partial information:

    java.util.ArrayList<BSONObj> res = con.find("javadb", "customers", "$\"name\", $\"address.phone\"", "");

Note: The select expects 4 parameters, so you should pass an empty filter if you want
all your results but unfiltered.

Contributing
------------

Want to contribute? Great! start by forking the repo on GitHub:

http://github.com/djondb/djondb_javadriver

* Clone your fork
* Do the hack/change you want
* Add some tests and be sure everything passes your tests
* If you add new functionality, document it in the README
* If necessary, rebase your commits into chunks, without errors
* Push the branch up to GitHub
* Send a pull request to djondb/djondb_javadriver 

There're other ways to contribute, if you need a new feature or something is missing just send your feedback
using the support group https://groups.google.com/forum/#!forum/djondb-support or using the feedback option
at the right hand of the djondb.com site.

