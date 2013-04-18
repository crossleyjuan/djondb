include_directories("${PROJECT_SOURCE_DIR}/third_party/includes")
include_directories("${PROJECT_SOURCE_DIR}/util")
include_directories("${PROJECT_SOURCE_DIR}/bson")
include_directories("${PROJECT_SOURCE_DIR}/cache")
include_directories("${PROJECT_SOURCE_DIR}/filesystem")

add_executable(test-filesystem main.cpp)
target_link_libraries (test-filesystem djon-filesystem djon-cache djon-bson djon-util ${LIBRARY_PREFIX}/gtest.lib ${LIBRARY_PREFIX}/gtest_main.lib ${PLATFORM_ESSENCIAL_LIBS})

add_test(test-filesystem test-filesystem)


