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

#include "Unix.h"
#include "JArgument.h"
#include "nativelib.h"

#include "name_pachler_nio_file_impl_Linux.h"

#include <sys/inotify.h>
#include <sys/vfs.h>
#include <errno.h>

JNIEXPORT jint JNICALL Java_name_pachler_nio_file_impl_Linux_inotify_1init
  (JNIEnv *, jclass)
{
	jint result = inotify_init();
	Unix_cacheErrno();
	return result;
}


JNIEXPORT jint JNICALL Java_name_pachler_nio_file_impl_Linux_inotify_1add_1watch
  (JNIEnv * env, jclass, jint fd, jstring name, jint mask)
{
	const char* namep = env->GetStringUTFChars(name, 0);
	jint result = inotify_add_watch(fd, namep, mask);
	Unix_cacheErrno();
	env->ReleaseStringUTFChars(name, namep);
	return result;
}

JNIEXPORT jint JNICALL Java_name_pachler_nio_file_impl_Linux_inotify_1rm_1watch
  (JNIEnv *, jclass, jint fd, jint wd)
{
	jint result = inotify_rm_watch(fd, wd);
	Unix_cacheErrno();
	return result;
}

JNIEXPORT jint JNICALL Java_name_pachler_nio_file_impl_Linux_statfs
  (JNIEnv* env, jclass clazz, jstring jpath, jobject jstatfs)
{
	if(jpath==0 || jstatfs==0)	// early exit if a parameter is null
	{
		nativelib_throwNullPointerException(env, "one of the parameters is null");
		return -1;
	}

	struct statfs stfs = {0};
	const char* path = env->GetStringUTFChars(jpath, 0);

	int result = statfs(path, &stfs);

	Unix_cacheErrno();

	env->ReleaseStringUTFChars(jpath, path);

	if(result != -1)
	{
		static jmethodID setMethodID = 0;

		JArgument arguments[] = {
			JArgument(stfs.f_type,     'J'),
			JArgument(stfs.f_bsize,    'J'),
			JArgument(stfs.f_blocks,   'J'),
			JArgument(stfs.f_bfree,    'J'),
			JArgument(stfs.f_bavail,   'J'),
			JArgument(stfs.f_files,    'J'),
			JArgument(stfs.f_ffree,    'J'),
			JArgument(stfs.f_namelen,  'I'),
		};

		if(setMethodID==0)
		{
			setMethodID = JArgument::getMethodID(env, "name/pachler/nio/file/impl/Linux$statfs", "set", "V", arguments);
		}


		if(setMethodID == 0){
			errno = ENOSYS;
			Unix_cacheErrno();
			return -1;
		}

		JArgument::callVoidMethod(env, jstatfs, setMethodID, arguments);
		if(env->ExceptionCheck())
			return -1;

	}

	return result;
}
