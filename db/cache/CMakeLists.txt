include_directories("${PROJECT_SOURCE_DIR}/util")
include_directories("${PROJECT_SOURCE_DIR}/bson")

add_definitions(-fPIC)
add_library(djon-cache cachemanager.cpp structure.cpp)

#target_link_libraries(djon-cache djon-bson djon-util)
