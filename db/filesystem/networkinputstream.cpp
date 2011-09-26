#include "networkinputstream.h"

#include "defs.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sstream>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

NetworkInputStream::NetworkInputStream(int clientSocket)
{
    _socket = clientSocket;
}

NetworkInputStream::~NetworkInputStream() {
    closeStream();
}

unsigned char NetworkInputStream::readChar() {
    CHECKSTATUS()
    unsigned char v;
    int readed = recv(_socket, &v, 1, 0);
    return v;
}

/* Reads 2 bytes in the input (little endian order) */
int NetworkInputStream::readInt () {
    CHECKSTATUS()
    int v = readChar() | readChar() << 8;
    return v;
}

/* Reads 4 bytes in the input (little endian order) */
long NetworkInputStream::readLong () {
    CHECKSTATUS()
    long v = readInt() | readInt() << 16;

    return v;
}

/* Reads a 4 byte float in the input */
float NetworkInputStream::readFloatIEEE () {
    CHECKSTATUS()
    float f;
    recv(_socket, &f, sizeof(f), 0);
    return f;
}

/* Reads a 8 byte double in the input */
double NetworkInputStream::readDoubleIEEE () {
    CHECKSTATUS()
    double d;
    recv(_socket, &d, sizeof(d), 0);
    return d;
}

/* Read a chars */
char* NetworkInputStream::readChars() {
    CHECKSTATUS()
    int len = readInt();
    char* res = readChars(len);
    return res;
}

std::string* NetworkInputStream::readString() {
    CHECKSTATUS()
    char* c = readChars();
    std::string* res = new std::string(c);
    free(c);
    return res;
}

char* NetworkInputStream::readChars(int length) {
    CHECKSTATUS()
    char* res = (char*)malloc(length+1);
    memset(res, 0, length+1);
    recv(_socket, res, length, 0);
    return res;
}

bool NetworkInputStream::eof() {
    CHECKSTATUS()
    return false;
}

void NetworkInputStream::closeStream() {
    close(_socket);
}

int NetworkInputStream::checkStatus() {
    fd_set read;
    FD_ZERO(&read);
    FD_SET(_socket, &read);
    timeval val;
    val.tv_sec = 10;
    val.tv_usec = 0;
    int result = select(_socket + 1, &read, NULL, NULL, &val);

    if (result < 0) {
        closeStream();
        return -1;
    }
    return 0;
}