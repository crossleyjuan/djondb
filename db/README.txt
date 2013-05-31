djondb
======

djondb is a NoSQL database implementation which follows the document datastore
model, the main goal is to achieve all the promises of NoSQL stores: performance,
scalability, etc, but focusing on consistency and multi document transactional.

If you need further information please refer to the site http://djondb.com there
you will find installation/compilation instructions, full documentation, etc.


Contributing
------------

Want to contribute? Great! start by forking the repo on GitHub:

http://github.com/djondb/djondb

* Clone your fork
* Do the hack/change you want
* Add some tests and be sure everything passes your tests
* If you add new functionality, document it in the README
* If necessary, rebase your commits into chunks, without errors
* Push the branch up to GitHub
* Send a pull request to djondb/djondb

There're other ways to contribute, if you need a new feature or something is missing just send your feedback
using the support group https://groups.google.com/forum/#!forum/djondb-support or using the feedback option
at the right hand of the djondb.com site.


Compiling
---------

To do the compilation you will need the following dependencies:

antlr3c Ant C runtime 3.2
google v8 javascript engine
boost libraries

to compile the google v8 use:

make --component=shared native

