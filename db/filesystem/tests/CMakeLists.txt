include_directories("${PROJECT_SOURCE_DIR}/third_party/includes")
include_directories("${PROJECT_SOURCE_DIR}/util")
include_directories("${PROJECT_SOURCE_DIR}/bson")
include_directories("${PROJECT_SOURCE_DIR}/cache")
include_directories("${PROJECT_SOURCE_DIR}/filesystem")

add_executable(test-filesystem main.cpp)
add_executable(test-buffers testBuffers.cpp)
IF (LINUX)
	target_link_libraries (test-filesystem djon-filesystem djon-cache djon-bson djon-util ${LIBRARY_PREFIX}gtest.a ${LIBRARY_PREFIX}gtest_main.a rt uuid)
	target_link_libraries (test-buffers djon-filesystem djon-cache djon-bson djon-util ${LIBRARY_PREFIX}gtest.a ${LIBRARY_PREFIX}gtest_main.a rt uuid)
ENDIF (LINUX)
IF (DARWIN)
	target_link_libraries (test-filesystem djon-filesystem djon-cache djon-bson djon-util ${LIBRARY_PREFIX}gtest.a ${LIBRARY_PREFIX}gtest_main.a)
	target_link_libraries (test-buffers djon-filesystem djon-cache djon-bson djon-util ${LIBRARY_PREFIX}gtest.a ${LIBRARY_PREFIX}gtest_main.a)
ENDIF (DARWIN)

IF (WIN32)
	target_link_libraries (test-filesystem djon-filesystem djon-cache djon-bson djon-util ${LIBRARY_PREFIX}gtest${LIBRARY_SUFFIX} ${LIBRARY_PREFIX}gtest_main${LIBRARY_SUFFIX})
	target_link_libraries (test-buffers djon-filesystem djon-cache djon-bson djon-util ${LIBRARY_PREFIX}gtest${LIBRARY_SUFFIX} ${LIBRARY_PREFIX}gtest_main${LIBRARY_SUFFIX})
ENDIF (WIN32)

add_test(test-filesystem test-filesystem)
add_test(test-buffers test-buffers)


