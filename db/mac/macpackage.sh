	rm -rf djondb.app
	mkdir -p djondb.app/

	# cp $HOME/workspace/djondb/db/libs/usr/lib/libdjon-util.0.dylib djondb.app/
	# cp $HOME/workspace/djondb/db/libs/usr/lib/libdjon-bson.0.dylib djondb.app/
	# cp $HOME/workspace/djondb/db/libs/usr/lib/libdjon-cache.0.dylib djondb.app/
	cp $HOME/workspace/djondb/db/libs/usr/lib/libdjon-client.0.dylib djondb.app/
	# cp $HOME/workspace/djondb/db/libs/usr/lib/libdjon-command.0.dylib djondb.app/
	# cp $HOME/workspace/djondb/db/libs/usr/lib/libdjon-db.0.dylib djondb.app/
	# cp $HOME/workspace/djondb/db/libs/usr/lib/libdjon-filesystem.0.dylib djondb.app/
	# cp $HOME/workspace/djondb/db/libs/usr/lib/libdjon-network.0.dylib djondb.app/
	# cp $HOME/workspace/djondb/db/libs/usr/lib/libdjon-service.0.dylib djondb.app/
	cp /usr/lib/libv8.dylib djondb.app/
	cp /usr/lib/libantlr3c.dylib djondb.app/
	cp $HOME/workspace/djondb/db/libs/usr/bin/djondbd djondb.app/
	cp $HOME/workspace/djondb/db/libs/usr/bin/djon-shell djondb.app/
	cp Info.plist djondb.app/
	cp ../djondb_cross.png djondb.app/djondbIcon.png


	# install_name_tool -id @executable_path/libdjon-util.0.dylib djondb.app/libdjon-util.0.dylib
	# install_name_tool -id @executable_path/libdjon-bson.0.dylib djondb.app/libdjon-bson.0.dylib
	# install_name_tool -id @executable_path/libdjon-cache.0.dylib djondb.app/libdjon-cache.0.dylib
	install_name_tool -id @executable_path/libdjon-client.0.dylib djondb.app/libdjon-client.0.dylib
	# install_name_tool -id @executable_path/libdjon-command.0.dylib djondb.app/libdjon-command.0.dylib
	# install_name_tool -id @executable_path/libdjon-db.0.dylib djondb.app/libdjon-db.0.dylib
	# install_name_tool -id @executable_path/libdjon-filesystem.0.dylib djondb.app/libdjon-filesystem.0.dylib
	# install_name_tool -id @executable_path/libdjon-network.0.dylib djondb.app/libdjon-network.0.dylib
	# install_name_tool -id @executable_path/libdjon-service.0.dylib djondb.app/libdjon-service.0.dylib
	install_name_tool -id @executable_path/libv8.dylib djondb.app/libv8.dylib
	install_name_tool -id @executable_path/libantlr3c.dylib djondb.app/libantlr3c.dylib
	install_name_tool -id @executable_path/djon-shell djondb.app/djon-shell
	install_name_tool -id @executable_path/djondbd djondb.app/djondbd

	# change of bson
	# install_name_tool -change /usr/lib/libdjon-util.0.dylib @executable_path/libdjon-util.0.dylib djondb.app/libdjon-bson.0.dylib

	# change of cache
	# install_name_tool -change /usr/lib/libdjon-util.0.dylib @executable_path/libdjon-util.0.dylib djondb.app/libdjon-cache.0.dylib
	# install_name_tool -change /usr/lib/libdjon-bson.0.dylib @executable_path/libdjon-bson.0.dylib djondb.app/libdjon-cache.0.dylib

	# change of filesystem
	# install_name_tool -change /usr/lib/libdjon-util.0.dylib @executable_path/libdjon-util.0.dylib djondb.app/libdjon-filesystem.0.dylib
	# install_name_tool -change /usr/lib/libdjon-bson.0.dylib @executable_path/libdjon-bson.0.dylib djondb.app/libdjon-filesystem.0.dylib
	# install_name_tool -change /usr/lib/libdjon-cache.0.dylib @executable_path/libdjon-cache.0.dylib djondb.app/libdjon-filesystem.0.dylib

	# change of db
	# install_name_tool -change /usr/lib/libdjon-util.0.dylib @executable_path/libdjon-util.0.dylib djondb.app/libdjon-db.0.dylib
	# install_name_tool -change /usr/lib/libdjon-bson.0.dylib @executable_path/libdjon-bson.0.dylib djondb.app/libdjon-db.0.dylib
	# install_name_tool -change /usr/lib/libdjon-cache.0.dylib @executable_path/libdjon-cache.0.dylib djondb.app/libdjon-db.0.dylib
	# install_name_tool -change /usr/lib/libdjon-filesystem.0.dylib @executable_path/libdjon-filesystem.0.dylib djondb.app/libdjon-db.0.dylib

	# change of command
	# install_name_tool -change /usr/lib/libdjon-util.0.dylib @executable_path/libdjon-util.0.dylib djondb.app/libdjon-command.0.dylib
	# install_name_tool -change /usr/lib/libdjon-bson.0.dylib @executable_path/libdjon-bson.0.dylib djondb.app/libdjon-command.0.dylib
	# install_name_tool -change /usr/lib/libdjon-cache.0.dylib @executable_path/libdjon-cache.0.dylib djondb.app/libdjon-command.0.dylib
	# install_name_tool -change /usr/lib/libdjon-filesystem.0.dylib @executable_path/libdjon-filesystem.0.dylib djondb.app/libdjon-command.0.dylib
	# install_name_tool -change /usr/lib/libdjon-db.0.dylib @executable_path/libdjon-db.0.dylib djondb.app/libdjon-command.0.dylib

	# change of network
	# install_name_tool -change /usr/lib/libdjon-util.0.dylib @executable_path/libdjon-util.0.dylib djondb.app/libdjon-network.0.dylib
	# install_name_tool -change /usr/lib/libdjon-bson.0.dylib @executable_path/libdjon-bson.0.dylib djondb.app/libdjon-network.0.dylib
	# install_name_tool -change /usr/lib/libdjon-cache.0.dylib @executable_path/libdjon-cache.0.dylib djondb.app/libdjon-network.0.dylib
	# install_name_tool -change /usr/lib/libdjon-filesystem.0.dylib @executable_path/libdjon-filesystem.0.dylib djondb.app/libdjon-network.0.dylib
	# install_name_tool -change /usr/lib/libdjon-db.0.dylib @executable_path/libdjon-db.0.dylib djondb.app/libdjon-network.0.dylib
	# install_name_tool -change /usr/lib/libdjon-command.0.dylib @executable_path/libdjon-command.0.dylib djondb.app/libdjon-network.0.dylib

	# change of driverbase
	# install_name_tool -change /usr/lib/libdjon-util.0.dylib @executable_path/libdjon-util.0.dylib djondb.app/libdjon-client.0.dylib
	# install_name_tool -change /usr/lib/libdjon-bson.0.dylib @executable_path/libdjon-bson.0.dylib djondb.app/libdjon-client.0.dylib
	# install_name_tool -change /usr/lib/libdjon-cache.0.dylib @executable_path/libdjon-cache.0.dylib djondb.app/libdjon-client.0.dylib
	# install_name_tool -change /usr/lib/libdjon-filesystem.0.dylib @executable_path/libdjon-filesystem.0.dylib djondb.app/libdjon-client.0.dylib
	# install_name_tool -change /usr/lib/libdjon-db.0.dylib @executable_path/libdjon-db.0.dylib djondb.app/libdjon-client.0.dylib
	# install_name_tool -change /usr/lib/libdjon-command.0.dylib @executable_path/libdjon-command.0.dylib djondb.app/libdjon-client.0.dylib
	# install_name_tool -change /usr/lib/libdjon-network.0.dylib @executable_path/libdjon-network.0.dylib djondb.app/libdjon-client.0.dylib
	install_name_tool -change /usr/lib/libantlr3c.dylib @executable_path/libantlr3c.dylib djondb.app/libdjon-client.0.dylib

	# change of service
	# install_name_tool -change /usr/lib/libdjon-util.0.dylib @executable_path/libdjon-util.0.dylib djondb.app/libdjon-service.0.dylib
	# install_name_tool -change /usr/lib/libdjon-bson.0.dylib @executable_path/libdjon-bson.0.dylib djondb.app/libdjon-service.0.dylib
	# install_name_tool -change /usr/lib/libdjon-cache.0.dylib @executable_path/libdjon-cache.0.dylib djondb.app/libdjon-service.0.dylib
	# install_name_tool -change /usr/lib/libdjon-filesystem.0.dylib @executable_path/libdjon-filesystem.0.dylib djondb.app/libdjon-service.0.dylib
	# install_name_tool -change /usr/lib/libdjon-db.0.dylib @executable_path/libdjon-db.0.dylib djondb.app/libdjon-service.0.dylib
	# install_name_tool -change /usr/lib/libdjon-command.0.dylib @executable_path/libdjon-command.0.dylib djondb.app/libdjon-service.0.dylib
	# install_name_tool -change /usr/lib/libdjon-network.0.dylib @executable_path/libdjon-network.0.dylib djondb.app/libdjon-service.0.dylib

	# change of djondbd
	# install_name_tool -change /usr/lib/libdjon-util.0.dylib @executable_path/libdjon-util.0.dylib djondb.app/djondbd
	# install_name_tool -change /usr/lib/libdjon-bson.0.dylib @executable_path/libdjon-bson.0.dylib djondb.app/djondbd
	# install_name_tool -change /usr/lib/libdjon-cache.0.dylib @executable_path/libdjon-cache.0.dylib djondb.app/djondbd
	# install_name_tool -change /usr/lib/libdjon-filesystem.0.dylib @executable_path/libdjon-filesystem.0.dylib djondb.app/djondbd
	# install_name_tool -change /usr/lib/libdjon-db.0.dylib @executable_path/libdjon-db.0.dylib djondb.app/djondbd
	# install_name_tool -change /usr/lib/libdjon-command.0.dylib @executable_path/libdjon-command.0.dylib djondb.app/djondbd
	# install_name_tool -change /usr/lib/libdjon-service.0.dylib @executable_path/libdjon-service.0.dylib djondb.app/djondbd
	# install_name_tool -change /usr/lib/libdjon-network.0.dylib @executable_path/libdjon-network.0.dylib djondb.app/djondbd
	install_name_tool -change /usr/lib/libdjon-client.0.dylib @executable_path/libdjon-client.0.dylib djondb.app/djondbd
	install_name_tool -change /usr/lib/libantlr3c.dylib @executable_path/libantlr3c.dylib djondb.app/djondbd

	# change of djon-shell
	# install_name_tool -change /usr/lib/libdjon-util.0.dylib @executable_path/libdjon-util.0.dylib djondb.app/djon-shell
	# install_name_tool -change /usr/lib/libdjon-bson.0.dylib @executable_path/libdjon-bson.0.dylib djondb.app/djon-shell
	# install_name_tool -change /usr/lib/libdjon-cache.0.dylib @executable_path/libdjon-cache.0.dylib djondb.app/djon-shell
	# install_name_tool -change /usr/lib/libdjon-filesystem.0.dylib @executable_path/libdjon-filesystem.0.dylib djondb.app/djon-shell
	# install_name_tool -change /usr/lib/libdjon-db.0.dylib @executable_path/libdjon-db.0.dylib djondb.app/djon-shell
	# install_name_tool -change /usr/lib/libdjon-command.0.dylib @executable_path/libdjon-command.0.dylib djondb.app/djon-shell
	# install_name_tool -change /usr/lib/libdjon-service.0.dylib @executable_path/libdjon-service.0.dylib djondb.app/djon-shell
	# install_name_tool -change /usr/lib/libdjon-network.0.dylib @executable_path/libdjon-network.0.dylib djondb.app/djon-shell
	install_name_tool -change /usr/lib/libdjon-client.0.dylib @executable_path/libdjon-client.0.dylib djondb.app/djon-shell
	install_name_tool -change /usr/local/lib/libv8.dylib @executable_path/libv8.dylib djondb.app/djon-shell
	install_name_tool -change /usr/lib/libantlr3c.dylib @executable_path/libantlr3c.dylib djondb.app/djon-shell

	# macdeployqt djondb.app -dmg
        # check paths with otool -L
        # install_name_tool -change <old path> <new path> <application>
