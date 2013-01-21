/*
 * Copyright 2008-2011 Uwe Pachler
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation. This particular file is
 * subject to the "Classpath" exception as provided in the LICENSE file
 * that accompanied this code.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include "nativelib.h"

#include <jni.h>
#include <cassert>

#if defined __UNIX__ || defined __unix__ || defined __APPLE__ || defined __linux__
#include "Unix.h"	// for Unix_init()
#endif

static const jint jniVersion = JNI_VERSION_1_4;


void nativelib_throwException(JNIEnv* env, const char* exName, const char* message)
{
	jclass cls = env->FindClass(exName);
	if(cls == 0)
		return;	// silently fail - this should never happen unless Sun/Oracle decides to remove this exception from the class library...
	env->ThrowNew(cls, message);
}

void nativelib_throwIllegalArgumentException(JNIEnv* env, const char* message)
{
	nativelib_throwException(env, "java/lang/IllegalArgumentException", message);
}

void nativelib_throwNullPointerException(JNIEnv* env, const char* message)
{
	nativelib_throwException(env, "java/lang/NullPointerException", message);
}

void nativelib_throwOutOfMemoryError(JNIEnv* env, const char* message)
{
	nativelib_throwException(env, "java/lang/OutOfMemoryError", message);
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
#if defined __UNIX__ || defined __unix__ ||  defined __APPLE__ || defined __linux__
	Unix_init();
#endif
	return jniVersion;
}


JNIEXPORT void JNICALL JNI_OnUnload(JavaVM* vm, void* reserved)
{
#if defined __UNIX__ || defined __APPLE__ || defined __linux__
	Unix_shutdown();
#endif
}
