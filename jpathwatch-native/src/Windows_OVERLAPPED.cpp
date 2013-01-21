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

#include "Windows_OVERLAPPED.h"
#include "name_pachler_nio_file_impl_Windows_OVERLAPPED.h"
#include "windows_include.hpp"

static jfieldID peerFieldID = 0;

jlong Windows_OVERLAPPED_getPeer(JNIEnv* env, jobject overlapped)
{
	return env->GetLongField(overlapped, peerFieldID);
}


extern "C"
{
	JNIEXPORT jlong JNICALL Java_name_pachler_nio_file_impl_Windows_00024OVERLAPPED_allocatePeer
	  (JNIEnv *, jclass)
	{
		OVERLAPPED empty = {};
		OVERLAPPED* overlapped = new OVERLAPPED();
		*overlapped = empty;
		return reinterpret_cast<jlong>(overlapped);
	}

	/*
	 * Class:     name_pachler_nio_file_impl_Windows_OVERLAPPED
	 * Method:    deallocatePeer
	 * Signature: (J)V
	 */
	JNIEXPORT void JNICALL Java_name_pachler_nio_file_impl_Windows_00024OVERLAPPED_deallocatePeer
	  (JNIEnv *, jclass, jlong peer)
	{
		OVERLAPPED* overlapped = reinterpret_cast<OVERLAPPED*>(peer);
		delete overlapped;
	}

	/*
	 * Class:     name_pachler_nio_file_impl_Windows_OVERLAPPED
	 * Method:    setEventOnPeer
	 * Signature: (JJ)V
	 */
	JNIEXPORT void JNICALL Java_name_pachler_nio_file_impl_Windows_00024OVERLAPPED_setEventOnPeer
	  (JNIEnv *, jclass, jlong peer, jlong eventHandle)
	{
		OVERLAPPED* overlapped = reinterpret_cast<OVERLAPPED*>(peer);
		overlapped->hEvent = reinterpret_cast<HANDLE>(eventHandle);
	}

	/*
	 * Class:     name_pachler_nio_file_impl_Windows_OVERLAPPED
	 * Method:    setOffsetOnPeer
	 * Signature: (JJ)V
	 */
	JNIEXPORT void JNICALL Java_name_pachler_nio_file_impl_Windows_00024OVERLAPPED_setOffsetOnPeer
	  (JNIEnv *, jclass, jlong peer, jlong offset)
	{
		OVERLAPPED* overlapped = reinterpret_cast<OVERLAPPED*>(peer);

		overlapped->Offset = static_cast<DWORD>(offset);
		overlapped->OffsetHigh = static_cast<DWORD>(offset>>32);
	}

	/*
	 * Class:     name_pachler_nio_file_impl_Windows_OVERLAPPED
	 * Method:    initNative
	 * Signature: ()V
	 */
	JNIEXPORT void JNICALL Java_name_pachler_nio_file_impl_Windows_00024OVERLAPPED_initNative
	  (JNIEnv* env, jclass clazz)
	{
		peerFieldID = env->GetFieldID(clazz, "peer", "J");
	}
}
