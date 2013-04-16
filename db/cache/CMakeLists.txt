include_directories("${PROJECT_SOURCE_DIR}/util")
include_directories("${PROJECT_SOURCE_DIR}/bson")
include_directories("${PROJECT_SOURCE_DIR}/cache")

add_library(djon-cache cachemanager.cpp structure.cpp)

add_executable(test-cache tests/main.cpp)
target_link_libraries (test-cache djon-cache djon-bson djon-util cpptest rt uuid)
add_test(test-cache test-cache)

#target_link_libraries(djon-cache djon-bson djon-util)
