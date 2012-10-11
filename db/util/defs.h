#ifndef DEFS_H_INCLUDED
#define DEFS_H_INCLUDED

#ifdef _WIN32
#define WINDOWS
#endif
#ifdef __linux
#define LINUX
#endif

#ifndef _WIN32
#include "config.h"
#else
#define VERSION "0.120121010"
#define PACKAGE_VERSION "0.1"
#endif

#ifdef __MACH__ || __APPLE__ || TARGET_OS_MAC
#define MAC
#endif


// #define TESTING

#endif // DEFS_H_INCLUDED
