include_directories("${PROJECT_SOURCE_DIR}/third_party/includes")
include_directories("${PROJECT_SOURCE_DIR}/util")
include_directories("${PROJECT_SOURCE_DIR}/bson")

add_executable(test-bson main.cpp)
target_link_libraries (test-bson djon-bson djon-util ${LIBRARY_PREFIX}/gtest.lib ${LIBRARY_PREFIX}/gtest_main.lib ${PLATFORM_ESSENCIAL_LIBS})

add_test(test-bson test-bson)

