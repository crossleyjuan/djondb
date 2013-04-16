cmake_minimum_required (VERSION 2.6)

project (djondb)

IF ("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
	set(LINUX true)
ENDIF ("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")

IF (LINUX)
	IF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
	add_definitions(-fPIC)
	#SET_TARGET_PROPERTIES(djon-client PROPERTIES COMPILE_FLAGS "-fPIC")
	ENDIF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )

ENDIF (LINUX)


configure_file (
	"${PROJECT_SOURCE_DIR}/config.h.in"
	"${PROJECT_BINARY_DIR}/config.h"
	)

include_directories("${PROJECT_BINARY_DIR}")
set(CMAKE_INSTALL_PREFIX /usr)
#SET_TARGET_PROPERTIES(djon-command PROPERTIES COMPILE_FLAGS "-fPIC")

IF ( WIN32 )
	include_directories("${PROJECT_SOURCE_DIR}/third_party/pthreads")
	include_directories("${PROJECT_SOURCE_DIR}/third_party/boost")
	include_directories("${PROJECT_SOURCE_DIR}/third_party/libantlr3c-3.2/include")
ENDIF ( WIN32 )

add_subdirectory(util) 
add_subdirectory(bson) 
add_subdirectory(cache) 
add_subdirectory(filesystem) 
add_subdirectory(db) 
add_subdirectory(tx) 
add_subdirectory(command) 
add_subdirectory(network) 
add_subdirectory(service) 
add_subdirectory(driverbase) 
IF (NOT WIN32 )
	add_subdirectory(daemon) 
ENDIF ( NOT WIN32 )
IF (WIN32)
	add_subdirectory(djondb_win)
ENDIF (WIN32)
add_subdirectory(shell) 

