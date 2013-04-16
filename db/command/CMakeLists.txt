include_directories("${PROJECT_SOURCE_DIR}/util")
include_directories("${PROJECT_SOURCE_DIR}/bson")
include_directories("${PROJECT_SOURCE_DIR}/cache")
include_directories("${PROJECT_SOURCE_DIR}/filesystem")
include_directories("${PROJECT_SOURCE_DIR}/db")
include_directories("${PROJECT_SOURCE_DIR}/tx")
include_directories("${PROJECT_SOURCE_DIR}/command")

add_library(djon-command command.cpp commandreader.cpp commandresult.cpp commandwriter.cpp findcommand.cpp insertcommand.cpp updatecommand.cpp removecommand.cpp commitcommand.cpp rollbackcommand.cpp dropnamespacecommand.cpp shutdowncommand.cpp showdbscommand.cpp shownamespacescommand.cpp)

#target_link_libraries(djon-command djon-db djon-filesystem djon-cache djon-bson djon-util)

add_executable(test-command tests/main.cpp)
target_link_libraries (test-command djon-command djon-tx djon-db djon-filesystem djon-cache djon-bson djon-util cpptest rt uuid)
add_test(test-command test-command)
