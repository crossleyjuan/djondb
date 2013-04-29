// *********************************************************************************************************************
// file:
// author: Juan Pablo Crossley (crossleyjuan@gmail.com)
// created:
// updated:
/*  License:
 * 
 * This file is part of the djondb project, for license information please refer to the LICENSE file,
 * the application and libraries are provided as-is and free of use under the terms explained in the file LICENSE
 * Its authors create this application in order to make the world a better place to live, but you should use it on
 * your own risks.
 * 
 * Also, be adviced that, the GPL license force the committers to ensure this application will be free of use, thus
 * if you do any modification you will be required to provide it for free unless you use it for personal use (you may
 * charge yourself if you want), bare in mind that you will be required to provide a copy of the license terms that ensures
 * this program will be open sourced and all its derivated work will be too.
 * 
 */

#include "buffer.h"
#include "buffermanager.h"
#include "util.h"
#include <gtest/gtest.h>

using namespace std;

TEST(testBuffers, testBuffers)
{

	std::string dataDir = getSetting("DATA_DIR");

	if (existFile((dataDir + "/test.log").c_str())) {
		removeFile((dataDir + "/test.log").c_str());
	}
	if (existFile((dataDir + "/test.trc").c_str())) {
		removeFile((dataDir + "/test.trc").c_str());
	}
	BufferManager* manager = new BufferManager("test");

	Buffer* buffer = NULL;
	char test[1001];

	memset(test, 0, 1001);
	memset(test, 'a', 1000);
  	for (int x = 0; x < 1000; x++) {
		buffer = manager->getCurrentBuffer(strlen(test));
		buffer->writeChars(test, strlen(test));
	};	
	delete manager;
}

