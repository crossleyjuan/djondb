// *********************************************************************************************************************
// file:
// author: Juan Pablo Crossley (crossleyjuan@gmail.com)
// created:
// updated:
// license:
// 
// This file is part of the djondb project, for license information please refer to the LICENSE file,
// the application and libraries are provided as-is and free of use under the terms explained in the file LICENSE
// Its authors create this application in order to make the world a better place to live, but you should use it on
// your own risks.
// 
// Also, be adviced that, the GPL license force the committers to ensure this application will be free of use, thus
// if you do any modification you will be required to provide it for free unless you use it for personal use (you may 
// charge yourself if you want), bare in mind that you will be required to provide a copy of the license terms that ensures
// this program will be open sourced and all its derivated work will be too.
// *********************************************************************************************************************

#include "service.h"
#include "networkservice.h"
#include <stdlib.h>

NetworkService* __service;

int service_restart() {
	return 0;
}

int service_startup() {
    if (__service != NULL) {
        __service->stop();
    }
    __service = new NetworkService();
    __service->start();
	return 0;
}

int service_shutdown() {
    __service->stop();
	 delete __service;
	 __service = NULL;
	return 0;
}

bool service_running() {
	if (__service) {
		return __service->running();
	} else {
		return false;
	}
}
