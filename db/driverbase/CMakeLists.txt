include_directories("${PROJECT_SOURCE_DIR}/util")
include_directories("${PROJECT_SOURCE_DIR}/bson")
include_directories("${PROJECT_SOURCE_DIR}/cache")
include_directories("${PROJECT_SOURCE_DIR}/filesystem")
include_directories("${PROJECT_SOURCE_DIR}/db")
include_directories("${PROJECT_SOURCE_DIR}/tx")
include_directories("${PROJECT_SOURCE_DIR}/command")
include_directories("${PROJECT_SOURCE_DIR}/network")

IF (NOT WIN32)
	add_library(djon-client SHARED djondbconnection.cpp djondbconnectionmanager.cpp dqlParser.cpp dqlLexer.cpp djonv8.cpp)
ENDIF (NOT WIN32)
IF (WIN32)
	include_directories("${PROJECT_SOURCE_DIR}/third_party/includes")

	add_library(djon-client djondbconnection.cpp djondbconnectionmanager.cpp dqlParser.cpp dqlLexer.cpp djonv8.cpp)
ENDIF (WIN32)

install (TARGETS djon-client 
	DESTINATION lib
	PERMISSIONS OWNER_READ GROUP_READ WORLD_READ OWNER_WRITE OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE)
install (FILES djondb_client.h DESTINATION include)

IF (LINUX)
	target_link_libraries (djon-client djon-network djon-command djon-tx djon-db djon-filesystem djon-cache djon-bson djon-util antlr3c pthread uuid rt v8)
ENDIF (LINUX)
IF (WIN32)
	target_link_libraries (djon-client djon-network djon-command djon-tx djon-db djon-filesystem djon-cache djon-bson djon-util ${LIBRARY_PREFIX}/antlr3c.lib ${LIBRARY_PREFIX}/pthreadVC2.lib ${LIBRARY_PREFIX}/v8.lib)
ENDIF (WIN32)
IF (DARWIN)
	target_link_libraries (djon-client djon-network djon-command djon-tx djon-db djon-filesystem djon-cache djon-bson djon-util antlr3c pthread v8)
ENDIF (DARWIN)
