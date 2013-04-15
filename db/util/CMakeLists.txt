include_directories("${PROJECT_SOURCE_DIR}")

IF ( LINUX )
add_definitions(-fPIC)
ENDIF (LINUX)

add_library(djon-util util.cpp  fileutil.cpp version.cpp threads.cpp dtime.cpp stringfunctions.cpp errorhandle.cpp logger.cpp datetime.cpp settings.cpp lock.cpp)
IF (WIN32)
	add_definitions(/clr )	
	#	set( refs "System.dll" )
	STRING(REPLACE "/EHsc" "/EHa" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
   STRING(REPLACE "/RTC1" "" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
	set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${PROJECT_SOURCE_DIR}/third_party/libs)
	#Rpcrt4.lib WINMM.LIB antlr3c.lib 
	LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/third_party/libs)
	set_target_properties(djon-util PROPERTIES VS_DOTNET_REFERENCES "System")
	find_library(antlr3c
		NAMES antlr3c
		HINTS "${PROJECT_SOURCE_DIR}/third_party/libs/Win32"
		)
	link_directories(${PROJECT_SOURCE_DIR}/third_party/libs)
	target_link_libraries (djon-util ${PROJECT_SOURCE_DIR}/third_party/libs/Win32/antlr3c.lib ${PROJECT_SOURCE_DIR}/third_party/libs/Win32/pthreadVC2.lib Rpcrt4.lib)
ENDIF ( WIN32 )

IF (NOT WIN32)
	target_link_libraries (djon-util antlr3c pthread)
ENDIF (NOT WIN32)

