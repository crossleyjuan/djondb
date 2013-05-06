include_directories("${PROJECT_SOURCE_DIR}/third_party/includes")
include_directories("${PROJECT_SOURCE_DIR}/util")
include_directories("${PROJECT_SOURCE_DIR}/bson")
include_directories("${PROJECT_SOURCE_DIR}/cache")

add_executable(test-cache main.cpp)
IF (LINUX)
	target_link_libraries (test-cache djon-cache djon-bson djon-util ${LIBRARY_PREFIX}gtest${LIBRARY_SUFFIX} ${LIBRARY_PREFIX}gtest_main${LIBRARY_SUFFIX} rt uuid)
ENDIF (LINUX)
IF (DARWIN)
	target_link_libraries (test-cache djon-cache djon-bson djon-util ${LIBRARY_PREFIX}gtest${LIBRARY_SUFFIX} ${LIBRARY_PREFIX}gtest_main${LIBRARY_SUFFIX})
ENDIF (DARWIN)

IF (WIN32)
	target_link_libraries (test-cache djon-cache djon-bson djon-util ${LIBRARY_PREFIX}gtest${LIBRARY_SUFFIX} ${LIBRARY_PREFIX}gtest_main${LIBRARY_SUFFIX})
ENDIF (WIN32)

add_test(test-cache test-cache)


