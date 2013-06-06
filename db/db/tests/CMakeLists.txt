include_directories("${PROJECT_SOURCE_DIR}/third_party/includes")
include_directories("${PROJECT_SOURCE_DIR}/util")
include_directories("${PROJECT_SOURCE_DIR}/bson")
include_directories("${PROJECT_SOURCE_DIR}/cache")
include_directories("${PROJECT_SOURCE_DIR}/filesystem")
include_directories("${PROJECT_SOURCE_DIR}/db")

add_executable(test-db main.cpp)
add_executable(test-indexp testIndexP.cpp)
IF (LINUX)
	target_link_libraries (test-db djon-db djon-filesystem djon-cache djon-bson djon-util ${LIBRARY_PREFIX}gtest.a ${LIBRARY_PREFIX}gtest_main.a rt uuid)
	target_link_libraries (test-indexp djon-db djon-filesystem djon-cache djon-bson djon-util ${LIBRARY_PREFIX}gtest.a ${LIBRARY_PREFIX}gtest_main.a rt uuid)
ENDIF (LINUX)
IF (DARWIN)
	target_link_libraries (test-db djon-db djon-filesystem djon-cache djon-bson djon-util ${LIBRARY_PREFIX}gtest.a ${LIBRARY_PREFIX}gtest_main.a)
	target_link_libraries (test-indexp djon-db djon-filesystem djon-cache djon-bson djon-util ${LIBRARY_PREFIX}gtest.a ${LIBRARY_PREFIX}gtest_main.a)
ENDIF (DARWIN)

IF (WIN32)
	target_link_libraries (test-db djon-db djon-filesystem djon-cache djon-bson djon-util ${LIBRARY_PREFIX}gtest${LIBRARY_SUFFIX} ${LIBRARY_PREFIX}gtest_main${LIBRARY_SUFFIX})
	target_link_libraries (test-indexp djon-db djon-filesystem djon-cache djon-bson djon-util ${LIBRARY_PREFIX}gtest${LIBRARY_SUFFIX} ${LIBRARY_PREFIX}gtest_main${LIBRARY_SUFFIX})
ENDIF (WIN32)

IF (NOT WIN32)
	add_custom_command(TARGET test-indexp
		POST_BUILD
		COMMAND cp ${PROJECT_SOURCE_DIR}/db/tests/*.csv ${PROJECT_BINARY_DIR}/db/tests/)
ENDIF (NOT WIN32)

IF (WIN32)
	add_custom_command(TARGET test-indexp
		POST_BUILD
		COMMAND copy ${PROJECT_SOURCE_DIR}/db/tests/*.csv ${PROJECT_BINARY_DIR}/db/tests/)
ENDIF (WIN32)

add_test(test-db test-db)
add_test(test-indexp test-indexp)


