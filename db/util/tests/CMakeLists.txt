include_directories("${PROJECT_SOURCE_DIR}/third_party/includes")
include_directories("${PROJECT_SOURCE_DIR}/util")

add_executable(test-util main.cpp)
target_link_libraries (test-util djon-util ${LIBRARY_PREFIX}/gtest.lib ${LIBRARY_PREFIX}/gtest_main.lib ${PLATFORM_ESSENCIAL_LIBS})

add_test(test-util test-util)
