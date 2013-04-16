include_directories("${PROJECT_SOURCE_DIR}/util")
include_directories("${PROJECT_SOURCE_DIR}/bson")
include_directories("${PROJECT_SOURCE_DIR}/cache")
include_directories("${PROJECT_SOURCE_DIR}/filesystem")

add_library(djon-filesystem bsoninputstream.cpp bsonoutputstream.cpp bsonbufferedobj.cpp bsonbufferedarrayobj.cpp bsonbufferedcontent.hpp fileinputoutputstream.cpp fileinputstream.cpp fileinputstreamw32.cpp mmapinputstream.cpp mmapinputoutputstream.cpp fileoutputstream.cpp networkinputstream.cpp networkoutputstream.cpp memorystream.cpp)

add_executable(test-filesystem tests/main.cpp)
target_link_libraries (test-filesystem djon-filesystem djon-cache djon-bson djon-util cpptest rt uuid)
add_test(test-filesystem test-filesystem)

#target_link_libraries(djon-filesystem djon-cache djon-bson djon-util)
