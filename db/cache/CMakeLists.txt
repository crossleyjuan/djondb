include_directories("${PROJECT_SOURCE_DIR}/util")
include_directories("${PROJECT_SOURCE_DIR}/bson")

IF ( WIN32 )
add_definitions(-fPIC)
ENDIF ( WIN32 )

add_library(djon-cache cachemanager.cpp structure.cpp)

#target_link_libraries(djon-cache djon-bson djon-util)
