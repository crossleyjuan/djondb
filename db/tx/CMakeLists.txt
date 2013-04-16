include_directories("${PROJECT_SOURCE_DIR}/util")
include_directories("${PROJECT_SOURCE_DIR}/bson")
include_directories("${PROJECT_SOURCE_DIR}/cache")
include_directories("${PROJECT_SOURCE_DIR}/filesystem")
include_directories("${PROJECT_SOURCE_DIR}/db")
include_directories("${PROJECT_SOURCE_DIR}/tx")

add_library(djon-tx basetransaction.cpp stdtransaction.cpp nodenotifier.cpp transactionmanager.cpp txbuffermanager.cpp txbuffer.cpp)

add_executable(test-tx tests/main.cpp)
target_link_libraries (test-tx djon-tx djon-db djon-filesystem djon-cache djon-bson djon-util cpptest rt uuid)
add_test(test-tx test-tx)

