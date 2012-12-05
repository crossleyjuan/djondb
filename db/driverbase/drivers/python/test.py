import pydjondb
con = pydjondb.DjondbConnectionManager.getConnection("localhost");

con.open();

con.insert("testpython", "testns", "{ 'name': 'John' }");

con.find("testpython", "testns", "");

