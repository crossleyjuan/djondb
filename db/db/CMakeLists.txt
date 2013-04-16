include_directories("${PROJECT_SOURCE_DIR}/util")
include_directories("${PROJECT_SOURCE_DIR}/bson")
include_directories("${PROJECT_SOURCE_DIR}/cache")
include_directories("${PROJECT_SOURCE_DIR}/filesystem")
include_directories("${PROJECT_SOURCE_DIR}/db")

add_library(djon-db bplusindex.cpp dbcontroller.cpp indexfactory.cpp indexmanager.cpp filterparser.cpp simpleexpression.cpp constantexpression.cpp unaryexpression.cpp expressionresult.cpp binaryexpression.cpp dbfilestream.cpp dbfileinputstream.cpp filterdefs.cpp filter_expressionLexer.cpp filter_expressionParser.cpp streammanager.cpp)

add_executable(test-db tests/main.cpp)
target_link_libraries (test-db djon-db djon-filesystem djon-cache djon-bson djon-util cpptest rt uuid)
add_test(test-db test-db)

#target_link_libraries(djon-db djon-filesystem djon-cache djon-bson djon-util)
