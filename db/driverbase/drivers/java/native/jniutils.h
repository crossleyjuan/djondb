/*
 * =====================================================================================
 *
 *       Filename:  jniutils.h
 *
 *    Description:  JNI Utils avoid rewriting of common code required at jni level, like
 *                  converting natives, etc
 *
 *        Version:  1.0
 *        Created:  09/16/2012 12:53:21 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Juan Pablo Crossley (crossleyjuan@gmail.com), 
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef JNIUTILS_INCLUDED_H
#define JNIUTILS_INCLUDED_H

#include <jni.h>

jobject convertToObject(JNIEnv* env, int i);
jobject convertToObject(JNIEnv* env, long i);
jobject convertToObject(JNIEnv* env, double i);
jstring convertToString(JNIEnv* env, const char* s);

#endif // JNIUTILS_INCLUDED_H
