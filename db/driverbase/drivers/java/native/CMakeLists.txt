include_directories("${PROJECT_SOURCE_DIR}/util")
include_directories("${PROJECT_SOURCE_DIR}/bson")
include_directories("${PROJECT_SOURCE_DIR}/cache")
include_directories("${PROJECT_SOURCE_DIR}/filesystem")
include_directories("${PROJECT_SOURCE_DIR}/db")
include_directories("${PROJECT_SOURCE_DIR}/command")
include_directories("${PROJECT_SOURCE_DIR}/network")
include_directories("${PROJECT_SOURCE_DIR}/driverbase")
include_directories("${PROJECT_SOURCE_DIR}/third_party/includes")
include_directories("${PROJECT_SOURCE_DIR}/third_party/includes/win32")

IF (NOT WIN32)
	add_library(djonjavadriver SHARED javadriver_wrap.cpp jniutils.cpp)
ENDIF (NOT WIN32)
IF (WIN32)
	add_library(djonjavadriver SHARED javadriver_wrap.cpp jniutils.cpp)
ENDIF (WIN32)

IF (LINUX)
	target_link_libraries (djonjavadriver djon-client djon-network djon-command djon-db djon-filesystem djon-cache djon-bson djon-util antlr3c pthread uuid rt)
ENDIF (LINUX)
IF (WIN32)
	target_link_libraries (djonjavadriver djon-client djon-network djon-command djon-db djon-filesystem djon-cache djon-bson djon-util ${LIBRARY_PREFIX}/antlr3c.lib ${LIBRARY_PREFIX}/pthreadVC2.lib)
ENDIF (WIN32)
IF (DARWIN)
	target_link_libraries (djonjavadriver djon-client djon-network djon-command djon-db djon-filesystem djon-cache djon-bson djon-util antlr3c pthread)
ENDIF (DARWIN)
