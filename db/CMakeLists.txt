cmake_minimum_required (VERSION 2.6)

project (djondb)

IF ("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
	set(LINUX true)
ENDIF ("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")


IF ("${CMAKE_SYSTEM_NAME}" MATCHES "Windows")
	set(WIN32 true)
	IF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
		set(PLATFORM_NAME "x64")
	ENDIF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
	IF( NOT CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
		set(PLATFORM_NAME "Win32")
	ENDIF( NOT CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
	set(LIBRARY_PREFIX "${PROJECT_SOURCE_DIR}/third_party/libs/${PLATFORM_NAME}/")
	set(LIBRARY_SUFFIX ".lib")
	set(PLATFORM_ESSENCIAL_LIBS, "")
ENDIF ("${CMAKE_SYSTEM_NAME}" MATCHES "Windows")

IF ("${CMAKE_SYSTEM_NAME}" MATCHES "Darwin")
	set(DARWIN true)
ENDIF ("${CMAKE_SYSTEM_NAME}" MATCHES "Darwin")

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

enable_testing()

include_directories("${PROJECT_BINARY_DIR}")
set(CMAKE_INSTALL_PREFIX /usr)

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

add_subdirectory(util/tests) 
add_subdirectory(bson/tests) 
add_subdirectory(cache/tests) 
add_subdirectory(filesystem/tests) 
add_subdirectory(db/tests) 
add_subdirectory(tx/tests) 
add_subdirectory(command/tests) 
add_subdirectory(network/tests) 
