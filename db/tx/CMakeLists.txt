include_directories("${PROJECT_SOURCE_DIR}/util")
include_directories("${PROJECT_SOURCE_DIR}/bson")
include_directories("${PROJECT_SOURCE_DIR}/cache")
include_directories("${PROJECT_SOURCE_DIR}/filesystem")
include_directories("${PROJECT_SOURCE_DIR}/db")

IF ( LINUX )
add_definitions(-fPIC)
ENDIF ( LINUX )

add_library(djon-tx basetransaction.cpp stdtransaction.cpp nodenotifier.cpp transactionmanager.cpp txbuffermanager.cpp txbuffer.cpp)
