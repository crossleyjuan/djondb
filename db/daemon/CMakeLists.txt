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

IF (NOT WIN32)
install (TARGETS djondbd DESTINATION /usr/bin)
ENDIF (NOT WIN32)

IF (WIN32)
install (TARGETS djondbd DESTINATION bin)
ENDIF (WIN32)

IF (LINUX)
	target_link_libraries (djondbd djon-service djon-network djon-client djon-command djon-tx djon-db djon-filesystem djon-cache djon-bson djon-util antlr3c pthread uuid rt)
ENDIF (LINUX)

IF (DARWIN)
	target_link_libraries (djondbd djon-service djon-network djon-client djon-command djon-tx djon-db djon-filesystem djon-cache djon-bson djon-util ${LIBRARY_PREFIX}antlr3c.a pthread)
	#target_link_libraries (djondbd djon-service djon-network djon-client djon-command djon-tx djon-db djon-filesystem djon-cache djon-bson djon-util antlr3c pthread)
ENDIF (DARWIN)
