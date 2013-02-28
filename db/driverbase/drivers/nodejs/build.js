#!/usr/bin/env node
var spawn = require('child_process').spawn,
	 fs = require('fs'),
	 path = require('path');

console.info("Architecture: " + process.arch);
console.info("Platform: " + process.platform);
//Parse args
var force = false;
var arch = process.arch,
	 platform = process.platform,
	 v8 = /[0-9]+\.[0-9]+/.exec(process.versions.v8)[0];
	 var args = process.argv.slice(2).filter(function(arg) {
			if (arg === '-f') {
				force = true;
				return false;
			} else if (arg.substring(0, 13) === '--target_arch') {
				arch = arg.substring(14);
			}
		return true;
	});

if (!{ia32: true, x64: true, arm: true}.hasOwnProperty(arch)) {
	console.error('Unsupported (?) architecture: `'+ arch+ '`');
	process.exit(1);
}

// Test for pre-built library
var modPath = platform+ '-'+ arch+ '-v8-'+ v8;
if (!force) {
	try {
		fs.statSync(path.join(__dirname, 'bin', modPath, 'djondb.node'));
		console.log('`'+ modPath+ '` exists; skipping build');
		return process.exit();
	} catch (ex) {}
}

function checkExists(path) {
	try {
		fs.statSync(path);
		return true;
	} catch (ex) {
		return false;
	}
}

function checkDependencies() {
	var libPath;
	var includePath;
	var djonLibrary;
	if (process.platform == "linux") {
		libPath = "/usr/lib";
		includePath = "/usr/include";
		djonLibrary = "libdjon-client.so";
	} else if (process.platform = "darwin") {
		libPath = "/usr/lib";
		includePath = "/usr/include";
		djonLibrary = "libdjon-client.so";
	}
	var exist = checkExists(path.join(libPath, djonLibrary));
	if (!exist) {
		console.error("djondb client library not found, please install the development package or server and try again");
		process.exit();
	}

	if (!checkExists(path.join(includePath, "node/node.h"))) {
		console.error("node development files not found, please install nodejs-dev package and try again.");
		process.exit();
	}
}

function configure(done) {
	// Configure
	spawn(
			process.platform === 'win32' ? 'node-gyp.cmd' : 'node-gyp',
			['configure'].concat(args),
			{customFds: [0, 1, 2]}).on('exit', 
				function(err) {
					if (err) {
						if (err === 127) {
							console.error(
								'node-gyp not found! Please upgrade your install of npm!. You will need to execute: \n npm install -g node-gyp.'
								);
						} else {
							console.error('Build failed');
						}
						return process.exit(err);
					} else {
						done();
					}
				});

}

//Build it
function build() {
	spawn(
			process.platform === 'win32' ? 'node-gyp.cmd' : 'node-gyp',
			['build'].concat(args),
			{customFds: [0, 1, 2]})
	.on('exit', function(err) {
		if (err) {
			if (err === 127) {
				console.error(
					'node-gyp not found! Please upgrade your install of npm!. You will need to execute: \n npm install -g node-gyp.'
					);
			} else {
				console.error('Build failed');
			}
			return process.exit(err);
		}
	});
}

checkDependencies();
configure(build);
