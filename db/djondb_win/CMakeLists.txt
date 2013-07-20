include_directories("${PROJECT_SOURCE_DIR}/util")
include_directories("${PROJECT_SOURCE_DIR}/bson")
include_directories("${PROJECT_SOURCE_DIR}/cache")
include_directories("${PROJECT_SOURCE_DIR}/filesystem")
include_directories("${PROJECT_SOURCE_DIR}/db")
include_directories("${PROJECT_SOURCE_DIR}/tx")
include_directories("${PROJECT_SOURCE_DIR}/command")
include_directories("${PROJECT_SOURCE_DIR}/network")
include_directories("${PROJECT_SOURCE_DIR}/service")

add_executable(djondb_win djondb_win.cpp stdafx.cpp)

#add_definitions(/clr )	
#	set( refs "System.dll" )
#STRING(REPLACE "/EHsc" "/EHa" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
#STRING(REPLACE "/RTC1" "" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
#STRING(REPLACE "/Zi" "/ZI" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${PROJECT_SOURCE_DIR}/third_party/libs)
#Rpcrt4.lib WINMM.LIB antlr3c.lib 
link_directories(${PROJECT_SOURCE_DIR}/third_party/libs)

install (TARGETS djondb_win DESTINATION bin)

set_target_properties(djondb_win PROPERTIES VS_DOTNET_REFERENCES "System")
set_target_properties(djondb_win PROPERTIES CMAKE_EXE_LINKER_FLAGS "-static")

IF (WIN32)
	SET( CMAKE_SHARED_LINKER_FLAGS ${CMAKE_SHARED_LINKER_FLAGS} "/NODEFAULTLIB:MSVCRTD.lib") 
ENDIF (WIN32)

target_link_libraries (djondb_win djon-service djon-network djon-command djon-tx djon-db djon-filesystem djon-cache djon-bson djon-util ${LIBRARY_PREFIX}/antlr3c.lib ${LIBRARY_PREFIX}/pthreadVC2.lib Rpcrt4.lib)

