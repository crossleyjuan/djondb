include_directories("${PROJECT_SOURCE_DIR}/util")
include_directories("${PROJECT_SOURCE_DIR}/bson")
include_directories("${PROJECT_SOURCE_DIR}/cache")
include_directories("${PROJECT_SOURCE_DIR}/filesystem")
include_directories("${PROJECT_SOURCE_DIR}/db")
include_directories("${PROJECT_SOURCE_DIR}/tx")
include_directories("${PROJECT_SOURCE_DIR}/command")
include_directories("${PROJECT_SOURCE_DIR}/network")

add_library(djon-network networkservice.cpp)

add_executable(test-network tests/main.cpp)
target_link_libraries (test-network djon-network djon-command djon-tx djon-db djon-filesystem djon-cache djon-bson djon-util cpptest rt uuid)
add_test(test-network test-network)
