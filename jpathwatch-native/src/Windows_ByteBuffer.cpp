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

#include "Windows_ByteBuffer.h"

#include "name_pachler_nio_file_impl_Windows_ByteBuffer.h"

static jfieldID bufferAddressFieldID = 0;
static jfieldID bufferSizeFieldID = 0;

jbyte* Windows_ByteBuffer_getBytes(JNIEnv* env, jobject byteBuffer)
{
	jlong address = env->GetLongField(byteBuffer, bufferAddressFieldID);
	return reinterpret_cast<jbyte*>(address);
}

jsize Windows_ByteBuffer_getSize(JNIEnv* env, jobject byteBuffer)
{
	return env->GetIntField(byteBuffer, bufferSizeFieldID);
}

extern "C"
{
	/*
	 * Class:     name_pachler_nio_file_impl_Windows_ByteBuffer
	 * Method:    allocate
	 * Signature: (I)J
	 */
	JNIEXPORT jlong JNICALL Java_name_pachler_nio_file_impl_Windows_00024ByteBuffer_allocate
	  (JNIEnv *, jclass, jint size)
	{
		return reinterpret_cast<jlong>(new char[size]);
	}

	/*
	 * Class:     name_pachler_nio_file_impl_Windows_ByteBuffer
	 * Method:    deallocate
	 * Signature: (J)V
	 */
	JNIEXPORT void JNICALL Java_name_pachler_nio_file_impl_Windows_00024ByteBuffer_deallocate
	  (JNIEnv *, jclass, jlong buffer)
	{
		delete[] reinterpret_cast<char*>(buffer);
	}

	/*
	 * Class:     name_pachler_nio_file_impl_Windows_ByteBuffer
	 * Method:    initNative
	 * Signature: ()V
	 */
	JNIEXPORT void JNICALL Java_name_pachler_nio_file_impl_Windows_00024ByteBuffer_initNative
	  (JNIEnv* env, jclass clazz)
	{
		bufferAddressFieldID = env->GetFieldID(clazz, "bufferAddress", "J");
		bufferSizeFieldID = env->GetFieldID(clazz, "bufferSize", "I");
	}
}
