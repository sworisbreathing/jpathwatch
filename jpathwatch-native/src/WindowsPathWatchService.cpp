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

#include "name_pachler_nio_file_impl_WindowsPathWatchService.h"

#include "Windows_ByteBuffer.h"

#include "windows_include.hpp"

static jmethodID FILE_NOTIFY_INFORMATIONhandlerMethodID = 0;

extern "C"
{

	JNIEXPORT void JNICALL Java_name_pachler_nio_file_impl_WindowsPathWatchService_initNative
	  (JNIEnv* env, jclass clazz)
	{
		FILE_NOTIFY_INFORMATIONhandlerMethodID = env->GetMethodID(clazz, "FILE_NOTIFY_INFORMATIONhandler", "(Lname/pachler/nio/file/impl/WindowsPathWatchService$WatchRecord;ILjava/lang/String;)V");
	}

	JNIEXPORT void JNICALL Java_name_pachler_nio_file_impl_WindowsPathWatchService_translateFILE_1NOTIFY_1INFORMATION
	  (JNIEnv* env, jobject jthis, jobject watchRecord, jobject byteBuffer, jint bufferSize)
	{
		jbyte* bytes = Windows_ByteBuffer_getBytes(env, byteBuffer);

		const jbyte* current = bytes;
		const jbyte* end = bytes + bufferSize;
		for(;;)
		{
			const FILE_NOTIFY_INFORMATION* fni = reinterpret_cast<const FILE_NOTIFY_INFORMATION*>(current);
			
			// check that all pointers are within the buffer's boundaries
			const jbyte* fniEnd = reinterpret_cast<const jbyte*>(fni->FileName) + fni->FileNameLength;
			bool fniValid = bytes <= current && current < end && fniEnd <= end;
			if(!fniValid){
				fprintf(stderr, "TRUNCATED: bytes=%p bufferSize=%d current=%p end=%p fni=%p fniEnd=%p FileName=%p FileNameLength=%d\n", bytes, bufferSize, current, end, fni, fniEnd, fni->FileName, fni->FileNameLength);
				fflush(stderr);
				return;	// should never happen, this indicates a truncated buffer
			}

			const jbyte* next = current + fni->NextEntryOffset;
			
			if(next > end)
				return;	// should never happen; this would indicate a truncated buffer
			
			const jchar* jcharFileName = reinterpret_cast<const jchar*>(fni->FileName);
			jstring fileName = env->NewString(jcharFileName, fni->FileNameLength/2);

			env->CallVoidMethod(jthis, FILE_NOTIFY_INFORMATIONhandlerMethodID, watchRecord, fni->Action, fileName);
			if(env->ExceptionCheck())
				return;

			env->DeleteLocalRef(fileName);
			fileName = 0;

			if(fni->NextEntryOffset == 0)
				return;

			current = next;
		}
	}
}
