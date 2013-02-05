/*
 * =====================================================================================
 *
 *       Filename:  testfile.cpp
 *
 *    Description:  G
 *
 *        Version:  1.0
 *        Created:  12/12/2012 07:14:58 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Juan Pablo Crossley (Cross), crossleyjuan@gmail.com
 *   Organization:  djondb
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
 * =====================================================================================
 */
#include "fileoutputstream.h"
#include "bsonoutputstream.h"
#include "fileinputstream.h"
#include "bsoninputstream.h"
#include "bson.h"
#include <sys/mman.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>


int ELEMENTS = 10000000;

void testSave() {
	FileOutputStream fos("test.dat", "wb+");
	for (int x = 0; x < ELEMENTS; x++) {
		fos.writeInt(x);
		fos.writeChars("test cross", 10);
	}
	fos.close();
}

void testLoad() {
	FileInputStream fis("test.dat", "rb+");

	char* s = (char*)malloc(11);
	while (!fis.eof()) {
		fis.readInt();
		fis.readChars(10, s);
	}
	fis.close();
}

int testwmmap(int N, bool advise, bool shared) {
	Logger* log = getLogger(NULL);
	log->startTimeRecord();
	int answer = 0;
	int fd = ::open("test.dat", O_RDONLY);
	size_t length = (10*ELEMENTS) +(8*ELEMENTS);
	char* buffer = (char*)malloc((10*ELEMENTS)+1);
	memset(buffer, 0, (10 * ELEMENTS) + 1);
	//	for Linux:
#ifdef __linux__
	char*  addr = reinterpret_cast<char *>(mmap(NULL, length, PROT_READ, MAP_FILE | (shared?MAP_SHARED:MAP_PRIVATE) | MAP_POPULATE , fd, 0));
#else
	char*  addr = reinterpret_cast<char *>(mmap(NULL, length, PROT_READ, MAP_FILE | (shared?MAP_SHARED:MAP_PRIVATE), fd, 0));
#endif
	char* initaddr = addr;    
	if (addr == MAP_FAILED) {
		cout<<"Data can't be mapped???"<<endl;
		return -1;
	}
	if(advise)
		if(madvise(addr,length,MADV_SEQUENTIAL|MADV_WILLNEED)!=0) 
			cerr<<" Couldn't set hints"<<endl;
	close(fd);
	cout << "N: " << N << endl;
	int s  = 0;
	char* text = (char*)malloc(11);
	for(int t = 0; t < N; ++t) {
		char* c;
		c = addr;
		int* n = (int*)c;
		s += *n;
		int x = *n;
		addr += sizeof(int);// moves to the next int
		addr += sizeof(__int32);
		char* temp = addr;
		memset(text, 0, 11);
		strncpy(buffer + (t * 10), temp, 10), 
		addr += 10;
	}
	munmap(initaddr,length);
	log->stopTimeRecord();

	DTime time = log->recordedTime();

   cout << "Buffer: " << strlen(buffer) << endl;
	cout << "Total secs: " << time.totalSecs() << endl;
	return s;
}

int main(int argc, char** arg) {
	testSave();
	//testLoad();
	testwmmap(ELEMENTS, true, false);
}
