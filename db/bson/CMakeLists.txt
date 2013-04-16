include_directories("${PROJECT_SOURCE_DIR}/util")
include_directories("${PROJECT_SOURCE_DIR}/bson")

add_library(djon-bson bsonobj.cpp bsonarrayobj.cpp bsonutil.cpp bsoncontent.cpp bsonparser.cpp)

#target_link_libraries(djon-bson djon-util)

add_executable(test-bson tests/main.cpp)
target_link_libraries (test-bson djon-bson djon-util cpptest rt uuid)
add_test(test-bson test-bson)

