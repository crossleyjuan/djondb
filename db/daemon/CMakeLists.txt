include_directories("${PROJECT_SOURCE_DIR}/util")
include_directories("${PROJECT_SOURCE_DIR}/bson")
include_directories("${PROJECT_SOURCE_DIR}/cache")
include_directories("${PROJECT_SOURCE_DIR}/filesystem")
include_directories("${PROJECT_SOURCE_DIR}/db")
include_directories("${PROJECT_SOURCE_DIR}/tx")
include_directories("${PROJECT_SOURCE_DIR}/command")
include_directories("${PROJECT_SOURCE_DIR}/network")
include_directories("${PROJECT_SOURCE_DIR}/service")

add_executable(djondbd main.cpp)

install (TARGETS djondbd DESTINATION bin)

IF (LINUX)
	target_link_libraries (djondbd djon-service djon-network djon-client djon-command djon-tx djon-db djon-filesystem djon-cache djon-bson djon-util antlr3c pthread uuid rt)
ENDIF (LINUX)

IF (DARWIN)
	target_link_libraries (djondbd djon-service djon-network djon-client djon-command djon-tx djon-db djon-filesystem djon-cache djon-bson djon-util antlr3c pthread)
ENDIF (DARWIN)
