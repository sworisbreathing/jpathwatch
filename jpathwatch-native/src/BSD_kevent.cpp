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

#include "name_pachler_nio_file_impl_BSD_kevent.h"

#include "BSD_kevent.h"
#include "nativelib.h"


static jweak jkeventClassWeakRef = 0;
static jmethodID jkeventConstructorMID = 0;
static jfieldID jkeventPeerFID = 0;



static void kevent_initNative(JNIEnv* env, jclass clazz)
{
	jkeventClassWeakRef = env->NewWeakGlobalRef(clazz);
	jkeventConstructorMID = env->GetMethodID(clazz, "<init>", "()V");
	jkeventPeerFID = env->GetFieldID(clazz, "peer", "J");
}




static void kevent_setUDataJobject(JNIEnv* env, struct kevent* ke, jobject obj)
{
	void* udata = ke->udata;
	if(udata != 0)
	{
		env->DeleteGlobalRef(reinterpret_cast<jobject>(udata));
		udata = 0;
	}
	if(obj != 0)
		udata = reinterpret_cast<void*>(env->NewGlobalRef(obj));
	ke->udata = udata;
}


JNIEXPORT void JNICALL Java_name_pachler_nio_file_impl_BSD_00024kevent_initNative
  (JNIEnv* env, jclass clazz)
{
	kevent_initNative(env, clazz);
}

JNIEXPORT jlong JNICALL Java_name_pachler_nio_file_impl_BSD_00024kevent_createPeer
  (JNIEnv *, jclass)
{
	struct kevent empty = {};
	struct kevent* peer = new struct kevent(empty);
	return reinterpret_cast<jlong>(peer);
}

JNIEXPORT void JNICALL Java_name_pachler_nio_file_impl_BSD_00024kevent_destroyPeer
  (JNIEnv* env, jclass, jlong peer)
{
	struct kevent* e = reinterpret_cast<struct kevent*>(peer);
	kevent_setUDataJobject(env, e, 0);
	delete e;
}

JNIEXPORT jlong JNICALL Java_name_pachler_nio_file_impl_BSD_00024kevent_get_1ident
  (JNIEnv* env, jobject jkevent)
{
	return BSD_Jkevent_getPeer(env, jkevent)->ident;
}

JNIEXPORT void JNICALL Java_name_pachler_nio_file_impl_BSD_00024kevent_set_1ident
  (JNIEnv* env, jobject jkevent, jlong ident)
{
	BSD_Jkevent_getPeer(env, jkevent)->ident = ident;
}

JNIEXPORT jshort JNICALL Java_name_pachler_nio_file_impl_BSD_00024kevent_get_1filter
  (JNIEnv* env, jobject jkevent)
{
	return BSD_Jkevent_getPeer(env, jkevent)->filter;
}


JNIEXPORT void JNICALL Java_name_pachler_nio_file_impl_BSD_00024kevent_set_1filter
  (JNIEnv* env, jobject jkevent, jshort filter)
{
	BSD_Jkevent_getPeer(env, jkevent)->filter = filter;
}

JNIEXPORT jshort JNICALL Java_name_pachler_nio_file_impl_BSD_00024kevent_get_1flags
  (JNIEnv* env, jobject jkevent)
{
	return BSD_Jkevent_getPeer(env, jkevent)->flags;
}


JNIEXPORT void JNICALL Java_name_pachler_nio_file_impl_BSD_00024kevent_set_1flags
  (JNIEnv* env, jobject jkevent, jshort flags)
{
	BSD_Jkevent_getPeer(env, jkevent)->flags = flags;
}


JNIEXPORT jint JNICALL Java_name_pachler_nio_file_impl_BSD_00024kevent_get_1fflags
  (JNIEnv* env, jobject jkevent)
{
	return BSD_Jkevent_getPeer(env, jkevent)->fflags;
}


JNIEXPORT void JNICALL Java_name_pachler_nio_file_impl_BSD_00024kevent_set_1fflags
  (JNIEnv* env, jobject jkevent, jint fflags)
{
	BSD_Jkevent_getPeer(env, jkevent)->fflags = fflags;
}


JNIEXPORT jlong JNICALL Java_name_pachler_nio_file_impl_BSD_00024kevent_get_1data
  (JNIEnv* env, jobject jkevent)
{
	return BSD_Jkevent_getPeer(env, jkevent)->data;
}

JNIEXPORT void JNICALL Java_name_pachler_nio_file_impl_BSD_00024kevent_set_1data
  (JNIEnv* env, jobject jkevent, jlong data)
{
	BSD_Jkevent_getPeer(env, jkevent)->data = data;
}

JNIEXPORT jobject JNICALL Java_name_pachler_nio_file_impl_BSD_00024kevent_get_1udata
  (JNIEnv* env, jobject jkevent)
{
	struct kevent* ke = BSD_Jkevent_getPeer(env, jkevent);
	void* udata = ke->udata;
	return env->NewLocalRef(reinterpret_cast<jobject>(udata));
}

JNIEXPORT void JNICALL Java_name_pachler_nio_file_impl_BSD_00024kevent_set_1udata
  (JNIEnv* env, jobject jkevent, jobject obj)
{
	struct kevent* ke = BSD_Jkevent_getPeer(env, jkevent);
	kevent_setUDataJobject(env, ke, obj);
}

jobject BSD_Jkevent_create(JNIEnv* env)
{
	jclass clazz = (jclass) env->NewLocalRef(jkeventClassWeakRef);
	jobject instance = env->NewObject(clazz, jkeventConstructorMID);
	env->DeleteLocalRef(clazz);
	return instance;
}

struct kevent* BSD_Jkevent_getPeer(JNIEnv* env, jobject jkevent){
	if(jkevent == 0)
		return 0;
	jlong peer = env->GetLongField(jkevent, jkeventPeerFID);
	return reinterpret_cast<struct kevent*>(peer);
}
