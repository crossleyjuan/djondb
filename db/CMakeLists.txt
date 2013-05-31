cmake_minimum_required (VERSION 2.6)

project (djondb)

IF ("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
	set(LINUX true)
ENDIF ("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")


IF ("${CMAKE_SYSTEM_NAME}" MATCHES "Windows")
	set(WIN32 true)

	set(LIBRARY_PREFIX "${PROJECT_SOURCE_DIR}/third_party/libs/${PLATFORM_NAME}/${CMAKE_BUILD_TYPE}")
	message("Library suffix:" ${LIBRARY_PREFIX})
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

SET(DJONDB_VERSION_MAJOR "0")
SET(DJONDB_VERSION_MINOR "3")
SET(DJONDB_VERSION_REVISION "0")
SET(DJONDB_VERSION "${DJONDB_VERSION_MAJOR}.${DJONDB_VERSION_MINOR}.${DJONDB_VERSION_REVISION}")

configure_file (
	"${PROJECT_SOURCE_DIR}/config.h.in"
	"${PROJECT_BINARY_DIR}/config.h"
	)

enable_testing()

include_directories("${PROJECT_BINARY_DIR}")

INCLUDE(InstallRequiredSystemLibraries)

#debian variables
SET(CPACK_DEBIAN_PACKAGE_NAME "djondb")
SET(CPACK_DEBIAN_PACKAGE_VERSION "${DJONDB_VERSION_MAJOR}.${DJONDB_VERSION_MINOR}.${DJONDB_VERSION_REVISION}")
SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libantlr3c-3.2-0, libuuid1 (>= 2.17.2), libstdc++6 (>= 4.5), libv8-dev (>= 3.1)")
SET(CPACK_DEBIAN_PACKAGE_DESCRIPTION "Enterprise NoSQL Server 
 djondb is a NoSQL document store created to be 
  easy to use and ready enterprise level. Your 
   data is save with djondb.")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "djondb <info@djondb.com>")
SET(CPACK_DEBIAN_PACKAGE_SECTION "database")

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "djondb installer")
SET(CPACK_PACKAGE_VENDOR "djondb")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/README.txt")
SET(CPACK_RESOURCE_FILE_README "${PROJECT_SOURCE_DIR}/README.txt")
SET(CPACK_RESOURCE_FILE_WELCOME "${PROJECT_SOURCE_DIR}/WELCOME.txt")
SET(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/../LICENSE.txt")
SET(CPACK_PACKAGE_VERSION_MAJOR "${DJONDB_VERSION_MAJOR}")
SET(CPACK_PACKAGE_VERSION_MINOR "${DJONDB_VERSION_MINOR}")
SET(CPACK_PACKAGE_VERSION_PATCH "${DJONDB_VERSION_REVISION}")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "djondb ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}")

IF(WIN32 AND NOT UNIX)
  # There is a bug in NSI that does not handle full unix paths properly. Make
  # sure there is at least one set of four (4) backlasshes.
  #SET(CPACK_PACKAGE_ICON "${CMake_SOURCE_DIR}/Utilities/Release\\\\InstallIcon.bmp")
  #SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\MyExecutable.exe")
  SET(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY}")
  SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\djondb.com")
  SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\djondb.com")
  SET(CPACK_NSIS_CONTACT "info@djondb.com")
  SET(CPACK_NSIS_MODIFY_PATH ON)
ELSE(WIN32 AND NOT UNIX)
  SET(CPACK_STRIP_FILES "djondb_win.exe")
  SET(CPACK_STRIP_FILES "djon_shell.exe")
  SET(CPACK_SOURCE_STRIP_FILES "")
ENDIF(WIN32 AND NOT UNIX)
SET(CPACK_PACKAGE_EXECUTABLES "djondb" "djondb")

IF (NOT WIN32)
	SET(CPACK_SET_DESTDIR ON)
ENDIF (NOT WIN32)

INCLUDE(CPack)

IF (WIN32)
	INSTALL(FILES ${LIBRARY_PREFIX}/pthreadVC2.dll
		           ${LIBRARY_PREFIX}/v8.dll
            DESTINATION bin)

	INSTALL(FILES ${PROJECT_SOURCE_DIR}/other/windows/djondb.conf
		DESTINATION bin 
	PERMISSIONS OWNER_READ GROUP_READ WORLD_READ OWNER_WRITE OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE)
ENDIF (WIN32)

IF (LINUX)
	INSTALL(FILES ${PROJECT_SOURCE_DIR}/other/linux/djondb.conf
		DESTINATION /etc
	PERMISSIONS OWNER_READ GROUP_READ WORLD_READ OWNER_WRITE OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE)
ENDIF (LINUX)

IF (DARWIN)
	INSTALL(FILES ${PROJECT_SOURCE_DIR}/other/osx/djondb.conf
		DESTINATION /etc
	PERMISSIONS OWNER_READ GROUP_READ WORLD_READ OWNER_WRITE OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE)
   install (FILES /usr/lib/libv8.dylib /usr/lib/libantlr3c.dylib
   	DESTINATION /usr/lib
   	PERMISSIONS OWNER_READ GROUP_READ WORLD_READ OWNER_WRITE OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE)
ENDIF (DARWIN)

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

IF (NOT WIN32)
IF (${CMAKE_BUILD_TYPE} MATCHES "Debug")
	add_subdirectory(util/tests) 
	add_subdirectory(bson/tests) 
	add_subdirectory(cache/tests) 
	add_subdirectory(filesystem/tests) 
	add_subdirectory(db/tests) 
	add_subdirectory(tx/tests) 
	add_subdirectory(command/tests) 
	add_subdirectory(driverbase/tests) 
	add_subdirectory(network/tests) 
ENDIF (${CMAKE_BUILD_TYPE}  MATCHES "Debug")
ENDIF (NOT WIN32)
