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

#include <jni.h>

#include "name_pachler_nio_file_impl_Unix_timespec.h"

#include "Unix_timespec.h"

#include <time.h>

static jfieldID jtimespecPeerFID = 0;


timespec* Unix_Jtimespec_getPeer(JNIEnv* env, jobject jtimespec)
{
	jlong peer = env->GetLongField(jtimespec, jtimespecPeerFID);
	return reinterpret_cast<timespec*>(peer);
}


JNIEXPORT void JNICALL Java_name_pachler_nio_file_impl_Unix_00024timespec_initNative
  (JNIEnv* env, jclass clazz)
{
	jtimespecPeerFID = env->GetFieldID(clazz, "peer", "J");
}

JNIEXPORT jlong JNICALL Java_name_pachler_nio_file_impl_Unix_00024timespec_createPeer
  (JNIEnv *, jclass)
{
	timespec* ts = new timespec;
	timespec emptyTs = {};
	*ts = emptyTs;
	return reinterpret_cast<jlong>(ts);
}

JNIEXPORT void JNICALL Java_name_pachler_nio_file_impl_Unix_00024timespec_destroyPeer
  (JNIEnv *, jclass, jlong peer)
{
	timespec* ts = reinterpret_cast<timespec*>(peer);
	delete ts;
}

JNIEXPORT jlong JNICALL Java_name_pachler_nio_file_impl_Unix_00024timespec_get_1tv_1sec
  (JNIEnv* env, jobject obj)
{
	const timespec* ts = Unix_Jtimespec_getPeer(env, obj);
	return ts->tv_sec;
}

JNIEXPORT jlong JNICALL Java_name_pachler_nio_file_impl_Unix_00024timespec_get_1tv_1nsec
  (JNIEnv* env, jobject obj)
{
	const timespec* ts = Unix_Jtimespec_getPeer(env, obj);
	return ts->tv_nsec;
}

JNIEXPORT void JNICALL Java_name_pachler_nio_file_impl_Unix_00024timespec_set_1tv_1sec
  (JNIEnv* env, jobject obj, jlong sec)
{
	timespec* ts = Unix_Jtimespec_getPeer(env, obj);
	ts->tv_sec = sec;
}

JNIEXPORT void JNICALL Java_name_pachler_nio_file_impl_Unix_00024timespec_set_1tv_1nsec
  (JNIEnv* env, jobject obj, jlong nsec)
{
	timespec* ts = Unix_Jtimespec_getPeer(env, obj);
	ts->tv_nsec = nsec;
}

JNIEXPORT void JNICALL Java_name_pachler_nio_file_impl_Unix_00024timespec_set
  (JNIEnv* env, jobject obj, jlong sec, jlong nsec)
{
	timespec* ts = Unix_Jtimespec_getPeer(env, obj);
	ts->tv_sec = sec;
	ts->tv_nsec = nsec;
}
