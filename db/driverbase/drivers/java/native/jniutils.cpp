/*
 * =====================================================================================
 *
 *       Filename:  jniutils.cpp
 *
 *    Description:  JNI Utils avoid rewriting of common code required at jni level, like
 *                  converting natives, etc
 *
 *        Version:  1.0
 *        Created:  09/16/2012 12:54:59 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Juan Pablo Crossley (crossleyjuan@gmail.com), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include "jniutils.h"
#include <jni.h>

jobject convertToObject(JNIEnv* env, int i) {
	jclass clazzInteger = env->FindClass("java/lang/Integer");
	jmethodID minitInteger = env->GetMethodID(clazzInteger, "<init>", "(I)V");

	jobject ointeger = env->NewObject(clazzInteger, minitInteger, (jint)i);

	return ointeger;
}

jobject convertToObject(JNIEnv* env, double d) {
	jclass clazzDouble = env->FindClass("java/lang/Double");
	jmethodID minitDouble = env->GetMethodID(clazzDouble, "<init>", "(D)V");

	jobject odouble = env->NewObject(clazzDouble, minitDouble, (jdouble)d);

	return odouble;
}

jobject convertToObject(JNIEnv* env, long l) {
	jclass clazzLong = env->FindClass("java/lang/Long");
	jmethodID minitLong = env->GetMethodID(clazzLong, "<init>", "(J)V");

	jobject olong = env->NewObject(clazzLong, minitLong, (jlong)l);

	return olong;
}

jstring convertToString(JNIEnv* env, const char* s) {
	return env->NewStringUTF(s);
}
