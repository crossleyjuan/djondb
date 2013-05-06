include_directories("${PROJECT_SOURCE_DIR}/util")
include_directories("${PROJECT_SOURCE_DIR}/bson")
include_directories("${PROJECT_SOURCE_DIR}/cache")
include_directories("${PROJECT_SOURCE_DIR}/filesystem")
include_directories("${PROJECT_SOURCE_DIR}/db")
include_directories("${PROJECT_SOURCE_DIR}/command")
include_directories("${PROJECT_SOURCE_DIR}/network")
include_directories("${PROJECT_SOURCE_DIR}/service")

add_executable(djondb_service AssemblyInfo.cpp djondb_serviceWinService.cpp ProjectInstaller.cpp stdafx.cpp)
	
add_definitions(/clr )	
STRING(REPLACE "/EHsc" "/EHa" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
STRING(REPLACE "/RTC1" "" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})

set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${PROJECT_SOURCE_DIR}/third_party/libs)
link_directories(${PROJECT_SOURCE_DIR}/third_party/libs)

install (TARGETS djondb_service DESTINATION .)
set_target_properties(djondb_service PROPERTIES VS_DOTNET_REFERENCES "System;System.Configuration.Install;System.Data;System.Management;System.ServiceProcess;System.Xml")
set_target_properties(djondb_service PROPERTIES CMAKE_EXE_LINKER_FLAGS "-static")

IF (WIN32)
	SET( CMAKE_SHARED_LINKER_FLAGS ${CMAKE_SHARED_LINKER_FLAGS} "/NODEFAULTLIB:MSVCRTD.lib") 
ENDIF (WIN32)

target_link_libraries (djondb_service djon-service djon-network djon-command djon-db djon-filesystem djon-cache djon-bson djon-util ${LIBRARY_PREFIX}/antlr3c.lib ${LIBRARY_PREFIX}/pthreadVC2.lib Rpcrt4.lib ${LIBRARY_PREFIX}/v8.lib)

