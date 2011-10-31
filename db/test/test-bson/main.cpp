#include <iostream>
#include "bsonobj.h"
#include <string>
#include <string.h>

using namespace std;

void testBSON() {
    cout << "Testing bson" << endl;
    BSONObj* obj = new BSONObj();
    // Add in
    obj->add("int", 1);
    obj->add("string", std::string("test"));
    obj->add("char*", (char*)"char*");
    obj->add("long", 1L);
    obj->add("double", 1.1);

    assert(obj->getInt("int") != NULL);
    assert(*obj->getInt("int") == 1);

    assert(obj->getString("string") != NULL);
    assert(((std::string*)obj->getString("string"))->compare("test") == 0);

    assert(obj->getChars("char*") != NULL);
    assert(strcmp((char*) obj->getChars("char*"), "char*") == 0);

    assert(obj->getLong("long") != NULL);
    assert(*obj->getLong("long") == 1L);

    assert(obj->getDouble("double") != NULL);
    assert(*obj->getDouble("double") == 1.1);

    delete obj;
}

void testCopyBSON() {
    cout << "Testing bson" << endl;
    BSONObj* objOrig = new BSONObj();
    // Add in
    objOrig->add("int", 1);
    objOrig->add("string", std::string("test"));
    objOrig->add("char*", (char*)"char*");
    objOrig->add("long", 1L);
    objOrig->add("double", 1.1);


    BSONObj* obj = new BSONObj(*objOrig);
    delete objOrig;
    objOrig = NULL;

    assert(obj->getInt("int") != NULL);
    assert(*obj->getInt("int") == 1);

    assert(obj->getString("string") != NULL);
    assert(((std::string*)obj->getString("string"))->compare("test") == 0);

    assert(obj->getChars("char*") != NULL);
    assert(strcmp((char*) obj->getChars("char*"), "char*") == 0);

    assert(obj->getLong("long") != NULL);
    assert(*obj->getLong("long") == 1L);

    assert(obj->getDouble("double") != NULL);
    assert(*obj->getDouble("double") == 1.1);

    delete obj;
}

int main()
{
    testBSON();

    testCopyBSON();

    return 0;
}