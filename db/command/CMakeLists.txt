include_directories("${PROJECT_SOURCE_DIR}/util")
include_directories("${PROJECT_SOURCE_DIR}/bson")
include_directories("${PROJECT_SOURCE_DIR}/cache")
include_directories("${PROJECT_SOURCE_DIR}/filesystem")
include_directories("${PROJECT_SOURCE_DIR}/db")
include_directories("${PROJECT_SOURCE_DIR}/tx")

add_definitions(-fPIC)
add_library(djon-command command.cpp commandreader.cpp commandresult.cpp commandwriter.cpp findcommand.cpp insertcommand.cpp updatecommand.cpp removecommand.cpp commitcommand.cpp rollbackcommand.cpp dropnamespacecommand.cpp shutdowncommand.cpp showdbscommand.cpp shownamespacescommand.cpp)

#target_link_libraries(djon-comand djon-db djon-filesystem djon-cache djon-bson djon-util)
