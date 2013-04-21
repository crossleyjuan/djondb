include_directories("${PROJECT_SOURCE_DIR}/third_party/includes")
include_directories("${PROJECT_SOURCE_DIR}/util")

add_executable(test-util main.cpp)
IF (NOT WIN32)
	target_link_libraries (test-util djon-util ${LIBRARY_PREFIX}gtest${LIBRARY_SUFFIX} ${LIBRARY_PREFIX}gtest_main${LIBRARY_SUFFIX} rt uuid) 
ENDIF (NOT WIN32)

IF (WIN32)
	target_link_libraries (test-util djon-util ${LIBRARY_PREFIX}gtest${LIBRARY_SUFFIX} ${LIBRARY_PREFIX}gtest_main${LIBRARY_SUFFIX}) 
ENDIF (WIN32)

add_test(test-util test-util)
