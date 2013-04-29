include_directories("${PROJECT_SOURCE_DIR}/util")
include_directories("${PROJECT_SOURCE_DIR}/bson")
include_directories("${PROJECT_SOURCE_DIR}/cache")

add_library(djon-filesystem buffermanager.cpp buffer.cpp bsoninputstream.cpp bsonoutputstream.cpp bsonbufferedobj.cpp bsonbufferedarrayobj.cpp bsonbufferedcontent.hpp fileinputoutputstream.cpp fileinputstream.cpp fileinputstreamw32.cpp mmapinputstream.cpp mmapinputoutputstream.cpp fileoutputstream.cpp networkinputstream.cpp networkoutputstream.cpp memorystream.cpp)

#target_link_libraries(djon-filesystem djon-cache djon-bson djon-util)
