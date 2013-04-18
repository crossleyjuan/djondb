include_directories("${PROJECT_SOURCE_DIR}/util")
include_directories("${PROJECT_SOURCE_DIR}/bson")
include_directories("${PROJECT_SOURCE_DIR}/cache")
include_directories("${PROJECT_SOURCE_DIR}/filesystem")
include_directories("${PROJECT_SOURCE_DIR}/db")
include_directories("${PROJECT_SOURCE_DIR}/tx")
include_directories("${PROJECT_SOURCE_DIR}/command")
include_directories("${PROJECT_SOURCE_DIR}/network")
include_directories("${PROJECT_SOURCE_DIR}/service")
include_directories("${PROJECT_SOURCE_DIR}/driverbase")

IF ( WIN32 )
	include_directories("${PROJECT_SOURCE_DIR}/third_party/includes")
	add_executable(djon-shell shell.cpp)
	STRING(REPLACE "/EHsc" "/EHa" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
	STRING(REPLACE "/RTC1" "" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
	STRING(REPLACE "/Zi" "/ZI" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
	set_target_properties(djon-shell PROPERTIES CMAKE_EXE_LINKER_FLAGS "-static")
	SET( CMAKE_SHARED_LINKER_FLAGS ${CMAKE_SHARED_LINKER_FLAGS} "/NODEFAULTLIB:MSVCRTD.lib") 
	target_link_libraries (djon-shell djon-client djon-service djon-network djon-command djon-tx djon-db djon-filesystem djon-cache djon-bson djon-util ${LIBRARY_PREFIX}/antlr3c.lib ${LIBRARY_PREFIX}/pthreadVC2.lib Rpcrt4.lib ${LIBRARY_PREFIX}/v8.lib)
ENDIF ( WIN32 )

IF (NOT WIN32)
	add_executable(djon-shell shell.cpp linenoise.c)
	target_link_libraries (djon-shell djon-client djon-service djon-network djon-command djon-tx djon-db djon-filesystem djon-cache djon-bson djon-util antlr3c pthread uuid rt v8)
ENDIF (NOT WIN32)

install (TARGETS djon-shell DESTINATION bin)



