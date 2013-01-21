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

#include <sys/inotify.h>

#include <algorithm>

#include "name_pachler_nio_file_impl_LinuxPathWatchService.h"


JNIEXPORT jint JNICALL Java_name_pachler_nio_file_impl_LinuxPathWatchService_translateInotifyEvents
  (JNIEnv * env, jobject obj, jbyteArray buffer, jint bufferPos, jint bufferSize)
{
	bufferPos = std::max(bufferPos, 0);
	bufferSize = std::min(bufferSize, env->GetArrayLength(buffer));
	bufferSize = std::max(bufferSize, 0);
	jbyte* bufferp = env->GetByteArrayElements(buffer, 0);
	jmethodID mid = env->GetMethodID(env->GetObjectClass(obj), "inotifyEventHandler", "(IIILjava/lang/String;)V");
	if(mid == 0)
		return bufferPos;

	while(bufferPos < bufferSize)
	{
		size_t bytesLeft = bufferSize - bufferPos;
		if(bytesLeft < sizeof(inotify_event))
		{
			// truncated, should never occur!
			break;
		}

		const inotify_event* ie = reinterpret_cast<const inotify_event*>(bufferp+bufferPos);

		if(bytesLeft < sizeof(inotify_event)+ie->len)
		{
			// truncated, should never occur!
			break;
		}

		jstring name = 0;
		if(ie->len > 0)
			name = env->NewStringUTF(ie->name);
		env->CallVoidMethod(obj, mid, ie->wd, ie->mask, ie->cookie, name);
		if(name != 0)
			env->DeleteLocalRef(name);

		bufferPos += sizeof(inotify_event)+ie->len;
	}

	env->ReleaseByteArrayElements(buffer, bufferp, JNI_ABORT);

	return bufferPos;
}
