include_directories("${PROJECT_SOURCE_DIR}/third_party/includes")
include_directories("${PROJECT_SOURCE_DIR}/util")
include_directories("${PROJECT_SOURCE_DIR}/bson")
include_directories("${PROJECT_SOURCE_DIR}/cache")
include_directories("${PROJECT_SOURCE_DIR}/filesystem")
include_directories("${PROJECT_SOURCE_DIR}/db")
include_directories("${PROJECT_SOURCE_DIR}/tx")
include_directories("${PROJECT_SOURCE_DIR}/command")
include_directories("${PROJECT_SOURCE_DIR}/network")

add_executable(test-network main.cpp)
IF (LINUX)
	target_link_libraries (test-network djon-network djon-command djon-tx djon-db djon-filesystem djon-cache djon-bson djon-util ${LIBRARY_PREFIX}gtest${LIBRARY_SUFFIX} ${LIBRARY_PREFIX}gtest_main${LIBRARY_SUFFIX} rt uuid ${LIBRARY_PREFIX}pthread)
ENDIF (LINUX)
IF (DARWIN)
	target_link_libraries (test-network djon-network djon-command djon-tx djon-db djon-filesystem djon-cache djon-bson djon-util ${LIBRARY_PREFIX}gtest${LIBRARY_SUFFIX} ${LIBRARY_PREFIX}gtest_main${LIBRARY_SUFFIX} ${LIBRARY_PREFIX}pthread)
ENDIF (DARWIN)

IF (WIN32)
	target_link_libraries (test-network djon-network djon-command djon-tx djon-db djon-filesystem djon-cache djon-bson djon-util ${LIBRARY_PREFIX}gtest${LIBRARY_SUFFIX} ${LIBRARY_PREFIX}gtest_main${LIBRARY_SUFFIX} ${LIBRARY_PREFIX}/pthreadVC2${LIBRARY_SUFFIX})
ENDIF (WIN32)

add_test(test-network test-network)

