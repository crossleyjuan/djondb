include_directories("${PROJECT_SOURCE_DIR}")

add_library(djon-util util.cpp  fileutil.cpp version.cpp threads.cpp dtime.cpp stringfunctions.cpp errorhandle.cpp logger.cpp datetime.cpp settings.cpp lock.cpp)
