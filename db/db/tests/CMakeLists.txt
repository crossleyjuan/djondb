include_directories("${PROJECT_SOURCE_DIR}/third_party/includes")
include_directories("${PROJECT_SOURCE_DIR}/util")
include_directories("${PROJECT_SOURCE_DIR}/bson")
include_directories("${PROJECT_SOURCE_DIR}/cache")
include_directories("${PROJECT_SOURCE_DIR}/filesystem")
include_directories("${PROJECT_SOURCE_DIR}/db")

add_executable(test-db main.cpp)
target_link_libraries (test-db djon-db djon-filesystem djon-cache djon-bson djon-util ${LIBRARY_PREFIX}/gtest.lib ${LIBRARY_PREFIX}/gtest_main.lib ${PLATFORM_ESSENCIAL_LIBS})

add_test(test-db test-db)


