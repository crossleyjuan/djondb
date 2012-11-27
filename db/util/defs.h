#ifndef DEFS_H_INCLUDED
#define DEFS_H_INCLUDED

#if (defined _WIN32 | _WIN64)
#define WINDOWS
#endif
#ifdef __linux
#define LINUX
#endif

#ifndef WINDOWS
#include "config.h"
#else
#define VERSION "0.120121125"
#define PACKAGE_VERSION "0.1"
#endif

#if (defined __MACH__ || __APPLE__ || TARGET_OS_MAC)
#define MAC
#endif


// Define architecture
#if (defined __amd64__ || __amd64 || __x86_64__ || __x86_64)
#define _64BITS
#else
#define _32BITS
#endif

#ifndef WINDOWS
#define __LONG64 long long
#else
#define __LONG64 __int64
#endif

// #define TESTING

#endif // DEFS_H_INCLUDED
