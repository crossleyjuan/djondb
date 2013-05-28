include_directories("${PROJECT_SOURCE_DIR}/util")

add_library(djon-bson bsonobj.cpp bsonarrayobj.cpp bsonutil.cpp bsoncontent.cpp bsonparser.cpp bson_grammarParser.cpp bson_grammarLexer.cpp)

#target_link_libraries(djon-bson djon-util)

